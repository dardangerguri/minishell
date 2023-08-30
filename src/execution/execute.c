/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhesso <jhesso@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/15 19:23:47 by jhesso            #+#    #+#             */
/*   Updated: 2023/08/30 04:32:14 by jhesso           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*	redirect_io()
*	Redirects input and output to either specified files or to a pipe
*	Close unused file descriptors
*/
// static void	redirect_io(t_tokens *cmd, int **pipe_fds, int not_first_cmd, int pipe_read)
// {
// 	if (cmd->fd_in > 0)
// 	{
// 		printf("duping STDIN_FILENO to fd_in\n");
// 		dup2(cmd->fd_in, STDIN_FILENO);
// 		close(pipe_read);
// 	}
// 	else if (not_first_cmd)
// 	{
// 		printf("duping STDIN_FILENO to pipe_read\n");
// 		dup2(pipe_read, STDIN_FILENO);
// 		if (cmd->fd_in != 0)
// 			close(cmd->fd_in);
// 	}
// 	if (cmd->fd_out > 0)
// 	{
// 		printf("duping STDOUT_FILENO to fd_out\n");
// 		dup2(cmd->fd_out, STDOUT_FILENO);
// 		close(pipe_fds[not_first_cmd][1]);
// 	}
// 	else if (cmd->next)
// 	{
// 		printf("duping STDOUT_FILENO to pipe_fds[1]\n");
// 		dup2(pipe_fds[not_first_cmd][1], STDOUT_FILENO);
// 		if (cmd->fd_out != 0)
// 			close(cmd->fd_out);
// 	}
// 	if (!cmd->next)
// 		close(pipe_fds[not_first_cmd][1]);
// 	if (!not_first_cmd && !check_builtin(cmd->command))
// 		close(pipe_read);
// }

static void	redirect_io(t_tokens *cmd, int **pipe_fds, int nb_cmd, int log)
{
	dprintf(log, "nb_cmd: %d cmd: %s\n", nb_cmd, cmd->command);
	if (cmd->fd_in > 0 && !check_builtin(cmd->command))
	{
		dprintf(log, "redirecting STDIN_FILENO into cmd->fd_in\n");
		dup2(cmd->fd_in, STDIN_FILENO);
		// close(pipe_read);
	}
	else if (nb_cmd)
	{
		dprintf(log, "redirecting STDIN_FILENO into pipe_fds[%d][0]\n", nb_cmd - 1);
		dup2(pipe_fds[nb_cmd - 1][0], STDIN_FILENO);
		close(pipe_fds[nb_cmd - 1][0]);
	}
	if (cmd->fd_out > 0)
	{
		dprintf(log, "redirecting STDOUT_FILENO into cmd->fd_out\n");
		dup2(cmd->fd_out, STDOUT_FILENO);
		// close(pipe_fds[nb_cmd][1]);
	}
	else if (cmd->next)
	{
		dprintf(log, "redirecting STDOUT_FILENO into pipe_fds[%d][1]\n", nb_cmd);
		dup2(pipe_fds[nb_cmd][1], STDOUT_FILENO);
		close(pipe_fds[nb_cmd][1]);
	}
	// if (!cmd->next)
	// 	close(pipe_fds[nb_cmd][1]);
	// if (!nb_cmd && !check_builtin(cmd->command))
	// 	close(pipe_read);
}

/*	child()
*	Child process executes the command and/or builtin function
*	redirects input and output to either specified files or to a pipe
*/
static void	child(t_tokens *cmd, t_minihell *mini, int not_first_cmd, int log)
{
	if (!cmd->command)
		exit(error_code) ;
	redirect_io(cmd, mini->pipe_fds, not_first_cmd, log);
	if (check_builtin(cmd->command))
	{
		execute_builtin(mini, check_builtin(cmd->command));
		exit(error_code);
	}
	dprintf(log, "running execve...\n");
	if (execve(cmd->command, cmd->argv, mini->env) == -1)
	{
		perror(strerror(errno));
		error_code = errno;
	}
}

/*	parent()
*	Parent process waits for all child processes to finish
*	Also saves the exit status of the last command (for $?)
*/
static void	parent(t_minihell *mini, int log)
{
	int		status;
	int		i;

	i = 0;
	usleep(1000);
	dprintf(log, "waiting for children to finish...\n");
	while(i < mini->nb_cmds)
	{
		dprintf(log, "PID: %d\n", mini->pids[i]);
		if (mini->pids[i] != -2)
			waitpid(mini->pids[i], &status, 0);
		dprintf(log, "i: %d\n", i);
		i++;
	}
	dprintf(log, "done waiting :)\n");
	if (WIFEXITED(status)) //! should be moved inside the while loop
		error_code = WEXITSTATUS(status);
	close_pipes(mini);
}

static void	print_fds(t_tokens *lst_tokens)
{
	while (lst_tokens)
	{
		printf("fd_in: %d, fd_out: %d\n", lst_tokens->fd_in, lst_tokens->fd_out);
		lst_tokens = lst_tokens->next;
	}
}

static int	open_log(void)
{
	int	fd;

	fd = open("log.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0)
		perror(strerror(errno));
	return (fd);
}

/*	execute()
*	Execute the command line
*	error_code is set to the exit status of the last command
*	Returns TRUE if execution was successful, FALSE otherwise
*/
bool	execute(t_minihell *minihell)
{ //! Realized a potential problem with heredoc: if more than one command has heredoc as input, only the last commands heredoc will be kept
	t_tokens	*head;
	int			i;
	// int			pipe_read;
	int			status;
	int			log;
	int			stdout;

	prepare_execution(minihell);
	print_fds(minihell->lst_tokens);
	head = minihell->lst_tokens;
	i = 0;
	stdout = dup(STDOUT_FILENO);
	// pipe_read = 0;
	log = open_log();
	if (log < 0)
		return (false);
	while (minihell->lst_tokens)
	{
		status = pipe(minihell->pipe_fds[i]);
		if (status == -1)
		{
			perror(strerror(errno));
			error_code = errno;
			break ;
		}
		if (minihell->lst_tokens->fd_in == -1)
		{
			free(minihell->lst_tokens->command);
			minihell->lst_tokens->command = NULL;
		}
		if (check_builtin(minihell->lst_tokens->command) && minihell->nb_cmds == 1)
		{
			redirect_io(minihell->lst_tokens, minihell->pipe_fds, i, log);
			execute_builtin(minihell, check_builtin(minihell->lst_tokens->command));
			dprintf(log, "builtin executed\n");
			dup2(stdout, STDOUT_FILENO);
		}
		else
			minihell->pids[i] = fork();
		if (minihell->pids[i] == -1)
		{
			perror(strerror(errno));
			error_code = errno;
			break ;
		}
		else if (minihell->pids[i] == 0)
			child(minihell->lst_tokens, minihell, i, log);
		else
			close(minihell->pipe_fds[i][1]);
		// pipe_read = minihell->pipe_fds[i][0];
		i++;
		minihell->lst_tokens = minihell->lst_tokens->next;
	}
	parent(minihell, log);
	// if (pipe_read != 0)
	// 	close(pipe_read);
	unlink(".heredoc.tmp");
	dprintf(log, "exit_code: %d\n", error_code);
	dprintf(log, "----------------------------\n");
	close(log);
	minihell->lst_tokens = head;
	return (true);
}
