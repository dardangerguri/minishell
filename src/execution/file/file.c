/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgerguri <dgerguri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/16 12:21:21 by jhesso            #+#    #+#             */
/*   Updated: 2023/09/01 14:35:46 by dgerguri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*	get_heredoc()
*	Open/create a temporary heredoc file and save user input to it
*	Return value: int (0 if success, -1 if failed)
*	Parameters:
*		(char *) delim: delimiter for heredoc
*/
static int	get_heredoc(char *delim)
{
	int		tmp_fd;
	int		stdin_fd;
	char	*line;

	tmp_fd = open(".heredoc.tmp", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (tmp_fd == -1)
		return (-1);
	stdin_fd = dup(STDIN_FILENO);
	while (1)
	{
		line = readline("> ");
		if (ft_strncmp(line, delim, ft_strlen(delim)) == 0 && \
		line[ft_strlen(delim)] == '\0')
		{
			free(line);
			break ;
		}
		ft_putendl_fd(line, tmp_fd);
		free(line);
	}
	close(stdin_fd);
	close(tmp_fd);
	return (0);
}

static int	open_file(char *filename, int mode)
{
	int	fd;

	if (mode == 0)
		fd = open(filename, O_RDONLY, 0644);
	else if (mode == 1)
	{
		fd = get_heredoc(filename);
		if (fd == 0)
			fd = open(".heredoc.tmp", O_RDONLY, 0644);
	}
	else if (mode == 2)
		fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else
		fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
	{
		ft_printf(2, "minishell: %s: %s\n", filename, strerror(errno));
		error_code = 1;
	}
	return (fd);
}

static int	open_input(t_minihell *minihell, t_tokens *lst_tokens, int i, bool *error_flag)
{
	if (!ft_strncmp(minihell->tokens[i], "<\0", 2))
	{
		if (lst_tokens->fd_in > 0)
			close(lst_tokens->fd_in);
		lst_tokens->fd_in = open_file(minihell->tokens[++i], 0);
		if (lst_tokens->fd_in == -1)
		{
			while (minihell->tokens[i + 1] && minihell->tokens[i][0] != '|')
				i++;
			*(error_flag) = true;
		}
	}
	else if (!ft_strncmp(minihell->tokens[i], "<<\0", 3))
	{
		if (lst_tokens->fd_in > 0)
			close(lst_tokens->fd_in);
		lst_tokens->fd_in = open_file(minihell->tokens[++i], 1);
	}
	return (i);
}

/*	open_file()
*	Opens the given file in the correct mode
*	Return value: int (file descriptor. -1 if failed)
*	Parameters:
*		(char *) filename: name of the file to be opened
*		(int) mode: mode for opening the file
*/
static int	open_output(t_minihell *minihell, t_tokens *lst_tokens, int i, bool *flag)
{
	if (!ft_strncmp(minihell->tokens[i], ">\0", 2))
	{
		if (lst_tokens->fd_out > 0)
			close(lst_tokens->fd_out);
		*(flag) = true;
		lst_tokens->fd_out = open_file(minihell->tokens[++i], 2);
	}
	else if (!ft_strncmp(minihell->tokens[i], ">>\0", 3) && *(flag) == false)
	{
		if (lst_tokens->fd_out > 0)
			close(lst_tokens->fd_out);
		lst_tokens->fd_out = open_file(minihell->tokens[++i], 3);
	}
	else if (!ft_strncmp(minihell->tokens[i], ">>\0", 3) && *(flag) == true)
	{
		if (lst_tokens->fd_out > 0)
			close(lst_tokens->fd_out);
		lst_tokens->fd_out = open_file(minihell->tokens[++i], 2);
	}
	return (i);
}

void	open_files(t_minihell *minihell, t_tokens *lst_tokens)
{
	static int	i = 0;
	bool		error_flag;
	bool		flag;

	lst_tokens->fd_in = 0;
	lst_tokens->fd_out = 0;
	error_flag = false;
	flag = false;
	while (minihell->tokens[i] && minihell->tokens[i][0] != '|' && error_flag == false)
	{
		if (minihell->tokens[i][0] == '<' && flag == false)
			i = open_input(minihell, lst_tokens, i, &error_flag);
		else if (minihell->tokens[i][0] == '>')
			i = open_output(minihell, lst_tokens, i, &flag);
		i++;
	}
	if (minihell->tokens[i] && minihell->tokens[i][0] == '|')
		i++;
	if (!minihell->tokens[i])
		i = 0;
	if (error_flag == false)
		append_command_path(minihell, lst_tokens);
	else
	{
		free(minihell->lst_tokens->command);
		minihell->lst_tokens->command = NULL;
	}
}
