/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tabuayya <tabuayya@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 17:15:12 by kabu-zee          #+#    #+#             */
/*   Updated: 2025/06/15 11:47:05 by tabuayya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	handle_exit_status(t_all *as, t_token *token, int *i)
{
	char	*env_value;
	int		start;
	int		end;

	if (token->value[*i] == '?')
	{
		(*i)++;
		env_value = ft_itoa(as->exit_status);
		start = *i - 1;
		end = *i;
		if (expand_variable(token, env_value, start, end) == -1)
			return (-1);
		free(env_value);
	}
	return (1);
}

int	extract_variable(t_all *as, char *value, int *i, char **var)
{
	int	start;
	int	end;

	start = *i;
	if (isdigit(value[*i]))
	{
		(*i)++;
		*var = ft_strdup("");
		if (!*var)
			exit_program(as, "Memory allocation failed", 1);
		return (start);
	}
	while (value[*i] && (isalnum(value[*i]) || value[*i] == '_'))
		(*i)++;
	end = *i;
	*var = ft_substr(value, start, end - start);
	if (!*var)
	{
		exit_program(as, "Memory allocation failed", 1);
	}
	return (start);
}

static char	*join_before_env_utils(char *before, char *var_value)
{
	char	*tmp;

	tmp = var_value;
	var_value = ft_strjoin(before, var_value);
	if (!var_value)
	{
		free(before);
		free(tmp);
		return (NULL);
	}
	free(before);
	free(tmp);
	return (var_value);
}

char	*join_before_env(t_token *token, char *env_value, int start)
{
	char	*before;
	char	*var_value;

	before = NULL;
	var_value = ft_strdup(env_value);
	if (!var_value)
		return (NULL);
	if (start > 1)
	{
		before = ft_substr(token->value, 0, start - 1);
		if (!before)
		{
			free(var_value);
			return (NULL);
		}
		var_value = join_before_env_utils(before, var_value);
	}
	return (var_value);
}

int	join_after_and_replace(t_token *token, char *var_value, int end, int len)
{
	char	*after;
	char	*tmp;

	after = NULL;
	if (end < len)
	{
		after = ft_substr(token->value, end, len);
		if (!after)
			return (free(var_value), -1);
		tmp = var_value;
		var_value = ft_strjoin(var_value, after);
		if (!var_value)
			return (free(after), free(tmp), -1);
		free(after);
		free(tmp);
	}
	free(token->value);
	token->value = var_value;
	return (0);
}
