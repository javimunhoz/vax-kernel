/*-
 * Copyright (c) 2013 Javier M. Mellid
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#define SVK_VERSION_STRING "Simple VAX Kernel (SVK) 0.1"
#define MAX_SHELL_COMMAND  80

unsigned char buffer_command[MAX_SHELL_COMMAND];

void command_help();

void command_ver();

void command_run_all_test_cases()
{
	run_all_tests();
}

void command_non_interlocked_mutex_test()
{
	run_non_interlocked_mutex_test();
}

void command_interlocked_mutex_test()
{
	run_interlocked_mutex_test();
}

void command_clock_test()
{
	run_clock_test();
}

void command_console_input_test()
{
	run_console_input_test();
}

void command_multitasking_on()
{
	run_multitask_on();
}

void command_multitasking_off()
{
	run_multitask_off();
}

struct command_t {
	unsigned char *name;
	void (*funct)(void);
	unsigned char *desc;
} command[]= {
	{ "help", command_help, "show help" },
        { "ver", command_ver, "show version" },
	{ "run_all_tests", command_run_all_test_cases, "run all test cases" },
	{ "run_nil_mutex_test", command_non_interlocked_mutex_test, "run non interlocked mutex test" },
	{ "run_il_mutex_test", command_interlocked_mutex_test, "run interlocked mutex test" },
	{ "run_clk_test", command_clock_test, "run clock test" },
	{ "run_cons_input_test", command_console_input_test, "run console input test" },
	{ "multitask_on", command_multitasking_on, "enable multitask, run proof of concept processes" },
	{ "multitask_off", command_multitasking_off, "switch to process 0, switch off multitask" },
	{ "", 0 }
};

void command_help() {
	struct command_t *c = command;

	kprintf("available commands:\n");

	while(c->name[0] != 0) {
		kprintf("%s - %s\n", c->name, c->desc);
		c++;
	}
}

void command_ver() {

        kprintf(SVK_VERSION_STRING "\n");
}

void dispatch(unsigned char *bufcom) {

	struct command_t *c = command;

	while(c->name[0] != 0) {

		if (strcmp(bufcom, c->name) == 0)
		{
			(*c->funct)();
			return;
		}

		c++;
	}

	kprintf("command not recognized\n");
}

void retrieve_command(unsigned char *bufcom)
{
	int i = MAX_SHELL_COMMAND;

	/* clear old command */
	while(i != 0) {
		bufcom[i] = 0;
		i--;
	}

	while(1) {
		if (consbuffer_peek_char() != (unsigned char)-1) {
			unsigned char c;
			c = consbuffer_del_char();
			if (c == 10) {
				kprintf("%c", c);
				break;
			}
			if (i == (MAX_SHELL_COMMAND-1))
				continue;
			kprintf("%c", c);
			bufcom[i] = c;
			i++;
		}
	}
}

void show_prompt() {
	kprintf("# ");
}

void shell_run() {
	while(1) {
		show_prompt();
		retrieve_command(buffer_command);
		dispatch(buffer_command);
	}
}

void i_am_process_0() {
	kprintf("running shell...\n");
	shell_run();
}
