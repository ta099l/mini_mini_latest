/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbaniatt <tbaniatt@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 00:52:04 by tbaniatt          #+#    #+#             */
/*   Updated: 2025/06/13 01:18:33 by tbaniatt         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	execute_env(char **args, t_envp *env)
{
	int	i;

	i = 0;
	if (args[1])
	{
		printf("env: too many arguments\n");
		return (1);
	}
	while (i < env->counter && env->tmp_envp[i])
	{
		if (ft_strchr(env->tmp_envp[i], '='))
			printf("%s\n", env->tmp_envp[i]);
		i++;
	}
	return (0);
}
