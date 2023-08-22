/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexing.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhesso <jhesso@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 15:46:22 by dgerguri          #+#    #+#             */
/*   Updated: 2023/08/22 23:48:20 by jhesso           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	**tokenize_cmd(char const *s, char c)
{
	char			**ret;
	unsigned int	i;
	unsigned int	row;
	unsigned int	word_count;

	if (!s)
		return (NULL);
	word_count = get_amount_of_words(s, c);
	ret = malloc(sizeof(char *) * (word_count + 1));
	if (!ret)
		return (NULL);
	i = 0;
	row = -1;
	while (++row < word_count)
	{
		while (s[i] == c)
			i++;
		if (s[i] != c)
			ret[row] = ft_substr(s, i, get_word_len(s, c, i));
		if (!ret[row])
			return (free_str_arr(ret));
		i = i + get_word_len(s, c, i);
	}
	ret[row] = NULL;
	return (ret);
}

bool	lexing(t_minihell *minihell, char *command_line)
{
	if (!char_checker(command_line, minihell, 0))
	{
		error_code = 258;
		return (false);
	}
	minihell->tokens = tokenize_cmd(command_line, ' ');
	if (!minihell->tokens)
	{
		free(command_line);
		return (false);
	}
	free(command_line);
	if (!syntax_checker(minihell->tokens))
	{
		free_str_arr(minihell->tokens);
		return (false);
	}
	minihell->tokens_bck = copy_str_arr(minihell->tokens);
	return (true);
}
