/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_expander_qoutes.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbaniatt <tbaniatt@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 17:15:12 by kabu-zee          #+#    #+#             */
/*   Updated: 2025/06/14 20:57:56 by tbaniatt         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	handle_single_quotes(const char *value, int *i, int in_double)
{
	if (value[*i] == '\'' && in_double == 0)
	{
		(*i)++;
		while (value[*i] && value[*i] != '\'')
			(*i)++;
		(*i)++;
		return (1);
	}
	return (0);
}

int	toggle_double_quote(int ch, int in_double)
{
	if (ch == '"')
		return (!in_double);
	return (in_double);
}

int	is_quoted(t_token *token)
{
	while (token)
	{
		token->quotes = 0;
		if (token->value && token->type == WORD)
		{
			if (ft_strchr(token->value, '\'') || ft_strchr(token->value, '"'))
			{
				token->quotes = 1;
			}
		}
		token = token->next;
	}
	return (0);
}

int	process_token_heredoc(t_all *as, t_token *token, t_envp *cp_envp)
{
	int	i;

	i = 0;
	while (token->value[i])
	{
		if (token->value[i] == '$')
		{
			if (process_dollar(as, token, &i, cp_envp) == -1)
				return (-1);
			i = 0;
			continue ;
		}
		i++;
	}
	return (0);
}
