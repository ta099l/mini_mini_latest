/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   excution.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kabu-zee <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 16:57:36 by kabu-zee          #+#    #+#             */
/*   Updated: 2025/06/15 16:57:37 by kabu-zee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

// Helper for input redirection
static void	handle_input_redirection(t_all *as, t_command *cmd, int prev_fd,
		int fd[2])
{
	int	fd_heredoc;
	int	fd_in;

	if (cmd->infile)
	{
		fd_in = open(cmd->infile, O_RDONLY);
		if (fd_in == -1)
		{
			close(fd[0]);
			close(fd[1]);
			exit_fork(as, "infile", 1);
		}
		dup2(fd_in, STDIN_FILENO);
		close(fd_in);
	}
	if (cmd->heredoc)
	{
		fd_heredoc = open(cmd->infile, O_RDONLY);
		if (fd_heredoc == -1)
			exit_fork(as, "open heredoc2", 1);
		dup2(fd_heredoc, STDIN_FILENO);
		close(fd_heredoc);
	}
	else if (prev_fd != -1)
		dup2(prev_fd, STDIN_FILENO);
}

// Helper for output redirection
static void	handle_output_redirection(t_all *as, t_command *cmd, int fd[2])
{
	int	fd_out;
	int	open_flags;

	if (cmd->outfile)
	{
		if (cmd->append)
			open_flags = O_WRONLY | O_CREAT | O_APPEND;
		else
			open_flags = O_WRONLY | O_CREAT | O_TRUNC;
		fd_out = open(cmd->outfile, open_flags, 0644);
		if (fd_out == -1)
			exit_fork(as, "outfile", 1);
		dup2(fd_out, STDOUT_FILENO);
		close(fd_out);
	}
	else if (cmd->next != NULL)
		dup2(fd[1], STDOUT_FILENO);
}

void	redirect_io(t_all *as, t_command *cmd, int prev_fd, int fd[2])
{
	handle_input_redirection(as, cmd, prev_fd, fd);
	handle_output_redirection(as, cmd, fd);
}
