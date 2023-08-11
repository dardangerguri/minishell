/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   removing_quotes.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgerguri <dgerguri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/10 21:42:59 by dgerguri          #+#    #+#             */
/*   Updated: 2023/08/11 14:45:23 by dgerguri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	len_without_quotes(char *str)
{
	char	c;
	int		len;
	int		flag;

	len = 0;
	flag = 0;
	while (str[len])
	{
		if (str[len] == '\'' || str[len] == '\"')
		{
			flag++;
			c = str[len++];
			while (str[len] && str[len] != c)
				len++;
			if (str[len] == '\0')
				break ;
			flag++;
		}
		len++;
	}
	return (len - flag);
}

char	*remove_quotes(char *str, int i, int j)
{
	char	*new_str;
	char	c;
	int		len;

	len = len_without_quotes(str);
	new_str = ft_calloc(sizeof(char), (len + 1));
	if (!new_str)
	{
		free(str);
		return (NULL);
	}
	while (str[j] && j < len)
	{
		if (str[j] == '\'' || str[j] == '\"')
		{
			c = str[j++];
			while (str[j] && str[j] != c)
				new_str[i++] = str[j++];
			if (str[j] || j == len)
				break ;
		}
		new_str[i++] = str[j++];
	}
	free(str);
	return (new_str);
}
