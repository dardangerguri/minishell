/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expanding.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhesso <jhesso@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/09 18:45:20 by dgerguri          #+#    #+#             */
/*   Updated: 2023/09/06 19:15:33 by jhesso           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*expand_str2()
{
	end = get_end_index(str, s + 1, 1);
		value = expand(str, envp, s, end);
		if (!value)
		{
			free(str);
			return (NULL);
		}
		new_str = insert_value(str, value, s, end);
		if (!new_str)
			return (NULL);
}

/*	expand_str()
*	do the actual expanding (replace the variable with its value)
*	Return value: char * (expanded string)
*	Parameters:
*		char *str: string to expand
*		char **envp: environment variables
*		int s: index of the '$' character
*/
char	*expand_str(char *str, char **envp, int s)
{
	char	*new_str;
	char	*value;
	int		end;

	if (str[s + 1] == '\'' || str[s + 1] == '\"')
	{
		new_str = insert_value(str, (char *)ft_calloc(1, 1), s, s + 1);
		if (!new_str)
			return (NULL);
	}
	else if (!str[s + 1])
	{
		new_str = ft_strdup("$");
		if (!new_str)
			malloc_error();
	}
	else if (str[s + 1] == '?')
		new_str = insert_value(str, ft_itoa(g_global.error_code), s, s + 2);
	else
		new_str = expand_str2();
	free(str);
	return (new_str);
}

/*	expand_quotes()
*	expand variables inside double quotes
*	Return value: char * (expanded string)
*	Parameters:
*		char *str: string to expand
*		char **envp: environment variables
*		int start: index of the first quote
*/
char	*expand_quotes(char *str, char **envp, int start)
{
	char	*new_str;

	new_str = ft_strdup(str);
	if (!new_str)
		return (NULL);
	start++;
	while (new_str[start] && new_str[start] != '\"')
	{
		if (new_str[start] == '$' && new_str[start + 1] != ' ')
		{
			new_str = expand_str(new_str, envp, start);
			if (!new_str)
			{
				free(str);
				return (NULL);
			}
		}
		start++;
	}
	free(str);
	return (new_str);
}

/*	expand_variables()
*	expand environment variables
*	Return value: char * (expanded string)
*	Parameters:
*		char *str: string to expand
*		char **envp: environment variables
*/
char	*expand_variables(char *str, char **envp)
{
	int	start;

	start = 0;
	while (str[start])
	{
		if (str[start] == '$' && str[start + 1])
		{
			str = expand_str(str, envp, start);
			if (!str)
				return (NULL);
		}
		else if (str[start] == '\"')
		{
			start++;
			str = expand_quotes(str, envp, start - 1);
			if (!str)
				return (NULL);
			start = get_end_index(str, start, 3);
		}
		else if (str[start] == '\'')
			start = get_end_index(str, start, 2);
		else
			start++;
	}
	return (str);
}
