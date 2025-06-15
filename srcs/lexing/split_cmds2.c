/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_cmds _utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kabu-zee <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:02:41 by kabu-zee          #+#    #+#             */
/*   Updated: 2025/06/15 17:02:43 by kabu-zee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

t_command	*new_command(t_all *as)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		exit_program(as, "MemoryDDD allocation failed", 1);
	cmd->args = NULL;
	cmd->infile = NULL;
	cmd->outfile = NULL;
	cmd->append = 0;
	cmd->next = NULL;
	cmd->heredoc = 0;
	cmd->executable = 1;
	return (cmd);
}

char	**add_arg(t_all *as, char **args, char *value)
{
	char	**new_args;
	int		i;
	int		count;

	i = 0;
	count = 0;
	while (args && args[count])
		count++;
	new_args = malloc(sizeof(char *) * (count + 2));
	if (!new_args)
		exit_program(as, "Memory allocation failed", 1);
	while (i < count)
	{
		new_args[i] = args[i];
		i++;
	}
	new_args[count] = ft_strdup(value);
	new_args[count + 1] = NULL;
	if (args)
		free(args);
	return (new_args);
}

void	append_command(t_command **cmd_list, t_command *new_cmd)
{
	t_command	*tmp;

	if (!*cmd_list)
	{
		*cmd_list = new_cmd;
		return ;
	}
	tmp = *cmd_list;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new_cmd;
}

static int	process_cmd(t_all *as, t_token **token, t_command **current_cmd,
		t_command **cmd_list)
{
	static int	flag;

	if ((*token)->type == WORD)
		handle_word_token(as, *current_cmd, *token);
	else if ((*token)->type == HEREDOC)
		handle_heredoc_token(as, *current_cmd, token);
	else if ((*token)->type == REDIR)
		handle_redir_token(as, *current_cmd, token, &flag);
	else if ((*token)->type == PIPE)
	{
		append_command(cmd_list, *current_cmd);
		if (!(*token)->next || ((*token)->next->type != WORD
				&& (*token)->next->type != REDIR
				&& (*token)->next->type != HEREDOC))
		{
			free_exit_status(as);
			return (1);
		}
		*current_cmd = new_command(as);
		if (!(*current_cmd))
			exit_program(as, "Memory allocation failed", 1);
	}
	return (0);
}

int	split_cmds(t_all *as, t_token *token, t_command **cmd_list)
{
	t_command	*current_cmd;

	*cmd_list = NULL;
	current_cmd = new_command(as);
	if (!current_cmd)
	{
		free(current_cmd);
		exit_program(as, "Memory allocation failed", 1);
	}
	while (token)
	{
		if (process_cmd(as, &token, &current_cmd, cmd_list))
			return (1);
		token = token->next;
	}
	append_command(cmd_list, current_cmd);
	return (0);
}

// int	split_cmds(t_all *as, t_token *token, t_command **cmd_list)
// {
// 	t_command	*current_cmd;
// 	int			flag;

// 	*cmd_list = NULL;
// 	current_cmd = new_command(as);
// 	flag = 0;
// 	if (!current_cmd)
// 	{
// 		free(current_cmd);
// 		exit_program(as, "Memory allocation failed", 1);
// 	}
// 	while (token)
// 	{
// 		if (token->type == WORD)
// 			handle_word_token(as, current_cmd, token);
// 		else if (token->type == HEREDOC)
// 			handle_heredoc_token(as, current_cmd, &token);
// 		else if (token->type == REDIR)
// 			handle_redir_token(as, current_cmd, &token, &flag);
// 		else if (token->type == PIPE)
// 		{
// 			append_command(cmd_list, current_cmd);
// 			if (!token->next || (token->next->type != WORD
// 					&& token->next->type != REDIR
// 					&& token->next->type != HEREDOC))
// 			{
// 				free_exit_status(as);
// 				return (1);
// 			}
// 			current_cmd = new_command(as);
// 			if (!current_cmd)
// 				exit_program(as, "Memory allocation failed", 1);
// 		}
// 		token = token->next;
// 	}
// 	append_command(cmd_list, current_cmd);
// 	return (0);
// }