/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_cmds.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tabuayya <tabuayya@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:02:35 by kabu-zee          #+#    #+#             */
/*   Updated: 2025/06/16 09:55:57 by tabuayya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

// void	print_commands(t_command *cmd_list)
// {
// 	int			i;
// 	int			cmd_num;
// 	t_command	*cmd;

// 	return ;
// 	cmd_num = 1;
// 	cmd = cmd_list;
// 	while (cmd)
// 	{
// 		printf("=== Command %d ===\n", cmd_num);
// 		if (cmd->args)
// 		{
// 			printf("Arguments:\n");
// 			for (i = 0; cmd->args[i]; i++)
// 				printf("  [%d]: %s\n", i, cmd->args[i]);
// 		}
// 		else
// 			printf("Arguments: None\n");
// 		if (cmd->infile)
// 			printf("Infile: %s\n", cmd->infile);
// 		else
// 			printf("Infile: None\n");
// 		if (cmd->outfile)
// 			printf("Outfile: %s (append: %s)\n", cmd->outfile,
// 				cmd->append ? "yes" : "no");
// 		else
// 			printf("Outfile: None\n");
// 		if (cmd->heredoc)
// 			printf("Herdoc : %d", cmd->heredoc);
// 		printf("\n");
// 		cmd = cmd->next;
// 		cmd_num++;
// 	}
// }

// Handles the actual opening/checking of the outfile
static void	check_outfile_access(t_all *as, t_command *current_cmd, int *flag)
{
	int	fd_out;
	int	flags;

	if (current_cmd->append)
		flags = O_WRONLY | O_CREAT | O_APPEND;
	else
		flags = O_WRONLY | O_CREAT | O_TRUNC;
	fd_out = open(current_cmd->outfile, flags, 0644);
	if (fd_out == -1)
	{
		perror("outfile");
		as->exit_status = 1;
		current_cmd->executable = 0;
		*flag = 1;
	}
	else
		close(fd_out);
}

// Main logic for handling the > or >> redirection
int	handle_outfile(t_all *as, t_command *current_cmd, t_token *token, int *flag)
{
	if (ft_strncmp(token->value, ">>", 2) == 0)
		current_cmd->append = 1;
	if (current_cmd->outfile)
		free(current_cmd->outfile);
	current_cmd->outfile = ft_strdup(token->next->value);
	if (!*flag)
		check_outfile_access(as, current_cmd, flag);
	return (0);
}

int	handle_infile(t_all *as, t_command *current_cmd, t_token *token, int *flag)
{
	int	fd_in;

	if (current_cmd->infile)
		free(current_cmd->infile);
	current_cmd->infile = ft_strdup(token->next->value);
	if (!*flag)
	{
		fd_in = open(current_cmd->infile, O_RDONLY);
		if (fd_in == -1)
		{
			perror("infile");
			as->exit_status = 1;
			current_cmd->executable = 0;
			*flag = 1;
		}
		else
			close(fd_in);
	}
	return (0);
}

int	handle_heredoc_token(t_all *as, t_command *current_cmd, t_token **token)
{
	if (!(*token)->next || (*token)->next->type != WORD)
	{
		free_exit_status(as);
		return (1);
	}
	current_cmd->heredoc = 1;
	if (current_cmd->infile)
		free(current_cmd->infile);
	current_cmd->infile = heredoc_cmd(as, (*token)->next->value, *token);
	*token = (*token)->next;
	return (0);
}

int	handle_redir_token(t_all *as, t_command *current_cmd, t_token **token,
		int *flag)
{
	if (!(*token)->next || (*token)->next->type != WORD)
		return (free_exit_status(as), 1);
	if (ft_strncmp((*token)->value, ">", 1) == 0)
		handle_outfile(as, current_cmd, *token, flag);
	else if (ft_strncmp((*token)->value, "<", 1) == 0)
		handle_infile(as, current_cmd, *token, flag);
	*token = (*token)->next;
	return (0);
}
