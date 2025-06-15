/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   excution_utils_3.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kabu-zee <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 16:58:07 by kabu-zee          #+#    #+#             */
/*   Updated: 2025/06/15 16:58:09 by kabu-zee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*get_rand(void)
{
	static int	counter = 0;
	char		*num_str;
	char		*name;
	char		*prefix;

	num_str = ft_itoa(counter++);
	if (!num_str)
		return (NULL);
	prefix = ft_strdup("/tmp/minishell_");
	if (!prefix)
	{
		free(num_str);
		return (NULL);
	}
	name = ft_strjoin(prefix, num_str);
	free(prefix);
	free(num_str);
	return (name);
}

static void	write_heredoc_line(t_all *as, t_token *token, char *line, int fd)
{
	int		len;
	t_token	tmp;

	tmp = (t_token){0};
	if (token->next && token->next->quotes == 0)
	{
		tmp.value = line;
		expand_var(as, &tmp, as->cp_envp, 1);
		line = tmp.value;
	}
	len = ft_strlen(line);
	write(fd, line, len);
	write(fd, "\n", 1);
	free(line);
}

static int	open_heredoc_file(t_all *as, char **out_name)
{
	int		fd;
	char	*name;

	name = get_rand();
	if (!name)
		exit_program(as, "open heredoc", 1);
	fd = open(name, O_RDWR | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		free(name);
		exit_program(as, "open heredoc", 1);
	}
	*out_name = name;
	return (fd);
}

static void	read_heredoc_input(t_all *as, t_token *token, const char *del,
		int fd)
{
	char	*line;
	size_t	n;

	n = ft_strlen(del);
	while (1)
	{
		line = readline("> ");
		if (!line)
			break ;
		if (ft_strlen(line) == n && ft_strncmp(line, del, n) == 0)
		{
			free(line);
			break ;
		}
		write_heredoc_line(as, token, line, fd);
	}
	close(fd);
}

char	*heredoc_cmd(t_all *as, char *del, t_token *token)
{
	char	*name;
	int		fd;

	fd = open_heredoc_file(as, &name);
	read_heredoc_input(as, token, del, fd);
	return (name);
}
