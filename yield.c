/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2025 Joel Pelaez Jorge
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdio.h>

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include "examples.h"

/**
 * Continucation function for yielable calls
 */
static int
continuation_func(lua_State *L, int status, __UNUSED lua_KContext ctx)
{
	/* Prints error if exists */
	if (status != LUA_OK && status != LUA_YIELD) {
		const char *error_msg = lua_tostring(L, -1);
		fprintf(stderr, "Lua error in continuation: %s\n", error_msg);
		lua_pop(L, -1);
	} else {
		/* Get continuation function information. */
		printf("Continuation function called. Status: %d\n", status);
		int results_count = lua_gettop(L);
		printf("Number of return value on the stack: %d\n",
		    results_count);
	}

	/* We don't return any value and it doesn't yield (coroutine will end).
	 */
	return 0;
}

/* A C function that prepares and executes a Lua chunk using lua_pcallk */
static int
call_lua_with_continuation(lua_State *L)
{
	const char *lua_code =
	    "print('Lua code running...');"
	    "coroutine.yield(1);"
	    "print('Lua code resumed... and running again.');"
	    "coroutine.yield(1);"
	    "print('Lua code resumed last time');";

	/* Load the Lua chunk onto the stack */
	if (luaL_loadstring(L, lua_code) != LUA_OK) {
		printf("Error loading Lua code: %s\n", lua_tostring(L, -1));
		return 0;
	}

	/*
	 * Call the Lua chunk with lua_pcallk
	 * 0 args, 0 results, no error handler, no context, with a continuation
	 * function
	 */
	printf("Calling lua_pcallk...\n");
	int status = lua_pcallk(L, 0, 0, 0, 0, continuation_func);

	/*
	 * This part of the code is only reached if lua_pcallk returns normally
	 * (not a yield) or if the continuation function returns.
	 */
	if (status == LUA_OK) {
		printf("lua_pcallk finished normally.\n");
	} else if (status == LUA_YIELD) {
		/* if the call is yield, it never reaches here */
		printf("lua_pcallk yielded. Control passed to C.\n");
	} else {
		/* Prints the error if exists. */
		printf("lua_pcallk error: %s\n", lua_tostring(L, -1));
	}

	return 0; /* Return control to the main coroutine manager (e.g.,
		   * lua_resume)
		   */
}

void
direct_call_to_coroutine(void)
{
	/* Create the main Lua state and load libraries */
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	/* Create a new thread (coroutine) to run our function in */
	lua_State *T = lua_newthread(L);

	/* Push the C function we want to run onto the new thread's stack */
	lua_pushcfunction(T, call_lua_with_continuation);

	int nresults;
	int status;

	do {
		/* Resume the coroutine to start or continue execution of
		 * 'call_lua_with_continuation' */
		status = lua_resume(T, L, 0, &nresults);

		/* Handle the call result inside the coroutine */
		if (status == LUA_OK) {
			/* The call terminates */
			printf("C code: coroutine ended.\n");
		} else if (status == LUA_YIELD) {
			/*
			 * Resume the coroutine.
			 *
			 * It will continue after coroutine.yield() on Lua code
			 * or will trigger the continuation_func when it runs C
			 * code (very last yield).
			 */
			printf("C code: coroutine yielded. Resuming...\n");
		}
	} while (status == LUA_YIELD);

	/* Clean up */
	lua_close(L);
}
