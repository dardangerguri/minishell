/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhesso <jhesso@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/09 17:15:21 by jhesso            #+#    #+#             */
/*   Updated: 2023/09/09 17:16:00 by jhesso           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	heredoc_sigint(int sign)
{
	(void)sign;
	close(g_global.heredoc_tmp);
	exit (1);
}

char	*get_num(char *name, int cmd)
{
	char	*nb;
	char	*tmp;

	nb = ft_itoa(cmd);
	tmp = ft_strjoin(name, nb);
	if (!tmp)
		return (NULL);
	while (access(tmp, F_OK) == 0)
	{
		free(nb);
		free(tmp);
		cmd++;
		nb = ft_itoa(cmd);
		tmp = ft_strjoin(name, nb);
		if (!tmp)
			return (NULL);
	}
	free(tmp);
	return (nb);
}
