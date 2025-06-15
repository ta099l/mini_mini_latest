/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kabu-zee <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 16:56:55 by kabu-zee          #+#    #+#             */
/*   Updated: 2025/06/15 16:56:56 by kabu-zee         ###   ########.fr       */
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
