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
 * C yielable function
 */
static int
kfunction(lua_State *L, __UNUSED int status, lua_KContext ctx)
{
	/* Internal counter received from ctx */
	lua_KContext x = ctx; /* used lua_KContext as an integer */

	/* Only do 3 times */
	if (x < 3) {
		x++;
		/* Push a string and yield value. */
		lua_pushfstring(L, "Wonderfull on %d call", x);

		/* Yields again updating ctx with new count value */
		return lua_yieldk(L, 1, x, kfunction);
	}

	/* Push final string. */
	lua_pushstring(L, "End");

	/* Return with one paramter on stack */
	return 1;
}

/**
 * C coroutine function.
 *
 * It receive the Lua state and push a new string and yield to Lua.
 */
static int
coroutine_entrypoint(lua_State *L)
{
	/* Send first value */
	lua_pushfstring(L, "Wonderfull entrypoint");

	/*
	 * Start yield with next parameters
	 *
	 * continue in current coroutine:                L
	 * one value was pushed:                         1
	 * continuation context will used as an counter: 0
	 * continuation function:                        kfunction
	 */
	return lua_yieldk(L, 1, 0, kfunction);
}

/**
 * Create a new Lua state with a custom function `coroutine_function`
 * as a
 */
static int
luaopen_module(lua_State *L)
{
	/* Create a new Lua thread (as a state) and set it as global in the
	 * orignal state */
	lua_State *n = lua_newthread(L);
	lua_setglobal(L, "coroutine_function");

	/* Set the entry function to the new thread as entry point. */
	lua_pushcfunction(n, coroutine_entrypoint);

	return 0;
}

/**
 * Create a new coroutine in C and call it from Lua (C yields -> Lua resumes).
 */
void
create_coroutine_in_c_and_call_it_from_lua()
{
	/* Create a new Lua state */
	lua_State *L = luaL_newstate();

	/* Load all standard libraries */
	luaL_openlibs(L);

	/* Prepare C coroutine (yields) */
	luaopen_module(L);

	/* Load Lua code */
	int error = luaL_loadstring(L,
	    /* Prints value from entrypoint */
	    "print(coroutine.resume(coroutine_function))\n"
	    /* Print the 3 expected values */
	    "print(coroutine.resume(coroutine_function))\n"
	    "print(coroutine.resume(coroutine_function))\n"
	    "print(coroutine.resume(coroutine_function))\n"
	    /* Prints the last value (c function didn't yield after it) */
	    "print(coroutine.resume(coroutine_function))\n"
	    /* Shows error becauses coroutine didn't yield again. */
	    "print(coroutine.resume(coroutine_function))");

	if (error) {
		fprintf(stderr, "%s", lua_tostring(L, -1));
		lua_pop(L, 1); /* pop error message from the stack */
	}

	/* Start principal Lua work from main thread (resume) */
	error = lua_pcall(L, 0, 1, 0);

	if (error) {
		fprintf(stderr, "%s", lua_tostring(L, -1));
		lua_pop(L, 1); /* pop error message from the stack */
	}

	lua_close(L);
}
