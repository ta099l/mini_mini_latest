#include "../includes/minishell.h"

// Helper for input redirection
static void	handle_input_redirection(t_all *as, t_command *cmd, int prev_fd, int fd[2])
{
	int	fd_heredoc;
	int	fd_in;

	if (cmd->heredoc)
	{
		fd_heredoc = open("/tmp/minishell_heredoc_tmp.txt", O_RDONLY);
		if (fd_heredoc == -1)
			exit_fork(as, "open heredoc2");
		dup2(fd_heredoc, STDIN_FILENO);
		close(fd_heredoc);
	}
	else if (cmd->infile)
	{
		fd_in = open(cmd->infile, O_RDONLY);
		if (fd_in == -1)
		{
			close(fd[0]);
			close(fd[1]);
			exit_fork(as, "infile");
		}
		dup2(fd_in, STDIN_FILENO);
		close(fd_in);
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
			exit_fork(as, "outfile");
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

// Helper: Write a line to heredoc file, expanding if needed
static void	write_heredoc_line(t_all *as, t_token *token, char *line, int fd)
{
	int		len;
	t_token	*tmp;
	char	*expanded;

	tmp = NULL;
	if (token->next && token->next->quotes == 0)
	{
		tmp = token;
		tmp->value = line;
		expand_var(as, tmp, as->cp_envp, 1);
		expanded = ft_strdup(tmp->value);
		free(tmp->value);
		tmp->value = NULL;
		line = expanded;
	}
	len = ft_strlen(line);
	write(fd, line, len);
	write(fd, "\n", 1);
	free(line);
}

// Main heredoc function, now much shorter
char	*heredoc_cmd(t_all *as, char *del, int n, t_token *token)
{
	int		fd;
	char	*line;

	fd = open("/tmp/minishell_heredoc_tmp.txt", O_RDWR | O_CREAT | O_TRUNC,
			0644);
	if (fd == -1)
		exit_program(as, "open heredoc", 1);
	while (1)
	{
		line = readline("> ");
		if (!line)
			break ;
		if (ft_strlen(line) == (size_t)n && ft_strncmp(line, del, n) == 0)
		{
			free(line);
			break ;
		}
		write_heredoc_line(as, token, line, fd);
	}
	close(fd);
	return (ft_strdup("/tmp/minishell_heredoc_tmp.txt"));
}
