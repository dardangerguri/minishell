/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgerguri <dgerguri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 17:17:54 by dgerguri          #+#    #+#             */
/*   Updated: 2023/09/06 20:02:50 by dgerguri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	check_validity(char *arg, int i)
{
	int	flag;

	flag = 0;
	if (!arg[i] || arg[i] == '=' || arg[i] == '+')
		return (invalid_variable(arg, 1));
	while (arg[i])
	{
		if (ft_isalpha(arg[i]))
			i++;
		else if (ft_isdigit(arg[i]) && !flag)
			return (invalid_variable(arg, 1));
		else if (ft_isdigit(arg[i]) && flag)
			i++;
		else if (arg[i] == '_')
		{
			flag = 1;
			i++;
		}
		else if (arg[i] == '=')
			break ;
		else
			return (invalid_variable(arg, 1));
	}
	return (0);
}

void	export_builtin(t_minihell *mini, int i, int argv_size)
{
	argv_size = count_strings(mini->cmds->argv);
	if (argv_size == 1)
		while (mini->env[i])
			printf("declare -x %s\n", mini->env[i++]);
	else
	{
		i = 1;
		while (mini->cmds->argv[i])
		{
			if (!check_validity(mini->cmds->argv[i], 0))
			{
				if (ft_strrchr(mini->cmds->argv[i], '='))
				{
					if (!already_exists(mini->env, mini->cmds->argv[i]))
						modify_variable(mini, mini->cmds->argv[i]);
					else
						mini->env = export_variable(mini->env, \
						mini->cmds->argv[i]);
					g_global.error_code = 0;
				}
			}
			i++;
		}
	}
}