#include "../includes/minishell.h"

int	is_valid_key(char *key)
{
	int	i;

	i = 1;
	if (!key || !ft_isalpha(key[0]))
		return (0);
	while (key[i])
	{
		if (!ft_isalnum(key[i]) && key[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

void	end_update(t_envp *env, char *entry, char **new_env)
{
	int	i;

	i = -1;
	while (++i < env->counter)
		new_env[i] = env->tmp_envp[i];
	new_env[i++] = entry;
	new_env[i] = NULL;
	free(env->tmp_envp);
	env->tmp_envp = new_env;
	env->counter++;
}

void	add_or_update_env(t_envp *env, char *key, char *value)
{
	int		i;
	char	*temp;
	char	*entry;
	char	**new_env;

	i = 0;
	temp = ft_strjoin(key, "=");
	entry = ft_strjoin(temp, value);
	free(temp);
	while (i < env->counter)
	{
		if (ft_strncmp(env->tmp_envp[i], key, ft_strlen(key)) == 0
			&& env->tmp_envp[i][ft_strlen(key)] == '=')
		{
			free(env->tmp_envp[i]);
			env->tmp_envp[i] = entry;
			return ;
		}
		i++;
	}
	new_env = malloc(sizeof(char *) * (env->counter + 2));
	end_update(env, entry, new_env);
}

void	if_true(char **args, t_envp *env, char *equal_sign, int i)
{
	int		key_len;
	char	*key;
	char	*value;

	key_len = equal_sign - args[i];
	key = ft_substr(args[i], 0, key_len);
	value = ft_strdup(equal_sign + 1);
	if (!is_valid_key(key))
		printf("export: `%s`: not a valid identifier\n", key);
	else
		add_or_update_env(env, key, value);
	free(key);
	free(value);
}

void	if_not(char **args, t_envp *env, int i)
{
	if (!is_valid_key(args[i]))
		printf("export: `%s`: not a valid identifier\n", args[i]);
	else
		add_or_update_env(env, args[i], "");
}
