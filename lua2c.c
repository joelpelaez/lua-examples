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
 * Create a new coroutine in Lua and resumes it in C (Lua yields -> C resumes).
 */
void
create_lua_coroutine_and_manage_in_c()
{
	/* Create a new Lua state */
	lua_State *L = luaL_newstate();

	/* Load all standard libraries */
	luaL_openlibs(L);

	/* Load Lua code */
	int error = luaL_loadstring(L,
	    "local co = coroutine.create(function()\n" /* Creates a coroutine */
	    "    print(\"Hello from Lua\")\n" /* Prints 'Hello from Lua' from
						 coroutine thread */
	    "    d = coroutine.yield(\"Send from Lua\")\n" /* Yields value 'Send
							      from Lua' and when
							      it resumes returns
							      'Send from C' */
	    "    print(d)\n" /* Prints resumed value */
	    "end)\n"
	    "return co" /* Return the created routine with the inner function */
	);

	if (error) {
		fprintf(stderr, "%s", lua_tostring(L, -1));
		lua_pop(L, 1); /* pop error message from the stack */
	}

	/* Call the Lua code and retrieve a coroutine */
	error = lua_pcall(L, 0, 1, 0);

	if (error != LUA_OK && error != LUA_YIELD) {
		fprintf(stderr, "%s", lua_tostring(L, -1));
		lua_pop(L, 1); /* pop error message from the stack */
		lua_close(L);
		return;
	}

	/* Get the coroutine as state */
	lua_State *state = lua_tothread(L, -1);
	int res = -1;

	do {
		/* Send the value and resume the coroutine */
		lua_pushstring(state, "Send from C");
		error = lua_resume(state, L, 1, &res);

		if (error != LUA_OK && error != LUA_YIELD) {
			fprintf(stderr, "%s", lua_tostring(L, -1));
			lua_pop(L, 1); /* pop error message from the stack */
		}

		/* Print received (yield) value */
		fprintf(stderr, "Received results: %d\n", res);
		for (; res > 0; res--) {
			const char *result = lua_tostring(state, 1);
			lua_pop(state, 1);

			fprintf(stderr, "Received value: %s\n", result);
		}
	} while (error ==
	    LUA_YIELD); /* If lua_resume returns LUA_YILED, call it again. */

	lua_close(L);
}
