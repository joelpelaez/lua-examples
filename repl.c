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
#include <string.h>

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include "examples.h"

/* Exit flag */
static int exit_loop = 0;

/* Lua callback for set exit flag to true. */
static int
quit_callback(__UNUSED lua_State *L)
{
	exit_loop = 1;
	return 0;
}

/* Create a simple Lua interpreter(REPL) */
void
run_lua_interpreter()
{
	char buff[256];
	int error;

	/* Create a new Lua State */
	lua_State *L = luaL_newstate();

	/* Load all Lua standard libraries */
	luaL_openlibs(L);

	/*
	 * Create a new table with the field `key` with value 'Value from C++'
	 * as global variable `val`
	 */
	lua_newtable(L);
	lua_pushstring(L, "Value from C++");
	lua_setfield(L, -2, "key");
	lua_setglobal(L, "val");

	/* Create a new string with value 'Hello from C++' as `native` global
	 * variable
	 */
	lua_pushstring(L, "Hello from C++");
	lua_setglobal(L, "native");

	/* Create a new cfunction as global `quit` */
	lua_pushcfunction(L, &quit_callback);
	lua_setglobal(L, "quit");

	/*
	 * Start loop
	 *
	 * Read from stdin until receive EOF
	 */
	while (fputs("> ", stdout), fgets(buff, sizeof(buff), stdin) != NULL) {
		/* Try to load code from buffer and execute it as Lua */
		error = luaL_loadbuffer(L, buff, strlen(buff), "line") ||
		    lua_pcall(L, 0, 1, 0);

		/*
		 * If a error was found, print it to stderr and remove from
		 * stack.
		 */
		if (error) {
			fprintf(stderr, "%s", lua_tostring(L, -1));
			lua_pop(L, 1); /* pop error message from the stack */
		}
		/* Code executed successfully */
		else {
			/* First check the Lua type from call result. */
			int r_type = lua_type(L, -1);

			switch (r_type) {
			case LUA_TSTRING: {
				/* If it's a string, retrieve the C string and
				 * print it. */
				const char *r_str = lua_tostring(L, -1);
				printf("String value: %s\n", r_str);

				/* Remove from stack. */
				lua_pop(L, 1);
				break;
			}
			default:
				break;
			}
		}

		/* If exit flag is true, quit loop. */
		if (exit_loop) {
			break;
		}
	}

	/* Close Lua */
	lua_close(L);
}
