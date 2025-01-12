/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dardangerguri <dardangerguri@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/08 16:15:23 by jhesso            #+#    #+#             */
/*   Updated: 2025/01/12 23:01:18 by dardangergu      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	handle_eof(t_minihell *minihell, struct termios t)
{
	(void)t;
	ft_putendl_fd("exit", STDOUT_FILENO);
	rl_clear_history();
	free_str_arr(minihell->env);
	t = set_termios(2);
	exit(minihell->error_code);
}

static void	handle_sigint(t_minihell *minihell)
{
	minihell->error_code = 1;
	g_global.signal_sigint = 0;
}

/*	minishell()
*	The main loop of minishell
*	Print a prompt, read user input and send that input to parsing
*/
void	minishell(t_minihell *minihell, char *command_line, bool ret)
{
	struct termios	t;

	while (1)
	{
		t = set_termios(1);
		signal(SIGINT, handle_signal);
		signal(SIGQUIT, SIG_IGN);
		command_line = readline(BLUE_BOLD "minishell$ " RESET_COLOR);
		if (command_line == NULL)
			handle_eof(minihell, t);
		signal(SIGQUIT, handle_cmd);
		signal(SIGINT, handle_cmd);
		if (g_global.signal_sigint == 1)
			handle_sigint(minihell);
		if (command_line && *command_line)
			add_history(command_line);
		ret = lexing(minihell, command_line);
		if (ret)
		{
			ret = parse(minihell);
			if (ret)
				execute(minihell);
			cleanup(minihell);
		}
	}
}

static void	init_minihell(t_minihell *minihell)
{
	minihell->tokens = NULL;
	minihell->env = NULL;
	minihell->pipe_fds = NULL;
	minihell->pids = NULL;
	minihell->nb_cmds = 0;
	minihell->heredocs = NULL;
	minihell->heredoc_nb = 0;
	minihell->error_code = 0;
	minihell->token_index = 0;
}

t_global	g_global;

int	main(int ac, char **av, char **envp)
{
	t_minihell	minihell;

	if (ac > 1)
	{
		ft_printf(2, "bash: %s: No such file or directory\n", av[1]);
		minihell.error_code = 127;
	}
	else
	{
		using_history();
		init_minihell(&minihell);
		init_env(&minihell, envp);
		minishell(&minihell, NULL, true);
		rl_clear_history();
	}
	return (0);
}
