/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dardangerguri <dardangerguri@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/25 20:55:01 by dgerguri          #+#    #+#             */
/*   Updated: 2025/01/12 23:02:36 by dardangergu      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	free_on_exit(t_minihell *mini)
{
	struct termios	t;

	(void)t;
	free_str_arr(mini->env);
	cleanup(mini);
	rl_clear_history();
	t = set_termios(2);
	exit(mini->error_code);
}

static void	overflows(long long int number, char *arg, bool *error)
{
	if (arg[0] == '-' && number == 0
		&& ft_strncmp(arg, "-9223372036854775808\0", 21))
		*error = false;
	else if (arg[0] != '-'
		&& number == -1 && ft_strncmp(arg, "9223372036854775807\0", 20))
		*error = false;
}

static int	arg_is_digit(char *arg)
{
	int	i;

	i = 0;
	if (arg[i] == '+' || arg[i] == '-')
		i++;
	while (arg[i])
	{
		if (!ft_isdigit(arg[i]))
			return (1);
		i++;
	}
	return (0);
}

static void	exit_checks(t_minihell *mini, int arg_size, int number, bool *error)
{
	if (arg_size == 2 && *error && !arg_is_digit(mini->cmds->argv[1]))
	{
		mini->error_code = number % 256;
		free_on_exit(mini);
	}
	else if (arg_size >= 2 && (!*error || arg_is_digit(mini->cmds->argv[1])))
	{
		ft_printf(2, "minishell: exit: %s: numeric argument required\n", \
			mini->cmds->argv[1]);
		mini->error_code = 255;
		free_on_exit(mini);
	}
	else if (arg_size >= 2)
	{
		ft_printf(2, "minishell: exit: too many arguments\n");
		mini->error_code = 1;
	}
	else
		free_on_exit(mini);
}

void	exit_builtin(t_minihell *minihell)
{
	bool		error;
	int			argv_size;
	long int	number;

	error = true;
	argv_size = count_strings(minihell->cmds->argv);
	number = 0;
	if (minihell->nb_cmds == 1)
		printf("exit\n");
	if (argv_size >= 2)
	{
		number = ft_atoi(minihell->cmds->argv[1]);
		overflows(number, minihell->cmds->argv[1], &error);
	}
	exit_checks(minihell, argv_size, number, &error);
}
