/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgerguri <dgerguri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/22 21:16:24 by jhesso            #+#    #+#             */
/*   Updated: 2023/09/14 18:28:21 by dgerguri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_signal(int sig)
{
	if (sig == SIGINT)
	{
		// if (minihell->error_code == 256 || minihell->error_code == 386)
		// 	minihell->error_code = 386;
		// else
		// {
			printf("\n");
			// minihell->error_code = 1;
			g_global.signal_sigint = 1;

		// }
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
		(void)sig;
		return ;
	}
}

void	handle_cmd(int sig)
{
	if (sig == SIGINT)
	{
		printf("\n");
		(void)sig;
	}
	else if (sig == SIGQUIT)
		printf("Quit: %d\n", sig);
	return ;
}
