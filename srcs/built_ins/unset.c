#include "../includes/minishell.h"

int is_valid_unset_key(char *key)
{
    int i;

    i = 0;
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

void remove_env_var(t_envp *env, const char *key)
{
    char **new_env;
    int i;
    int j;
    int len;

    i = 0;
    j = 0;
    len = ft_strlen(key);
    new_env = malloc(sizeof(char *) * (env->counter + 1));
    if (!new_env)
        return;
    while (i < env->counter)
    {
        if (ft_strncmp(env->tmp_envp[i], key, len) == 0 && env->tmp_envp[i][len] == '=')
            free(env->tmp_envp[i]);
        else
        {
            new_env[j++] = ft_strdup(env->tmp_envp[i]);
            free(env->tmp_envp[i]);
        }
        i++;
    }
    new_env[j] = NULL;
    remove_env_var_utils(env, new_env, j);
}

void remove_env_var_utils(t_envp *env, char **new_env, int j)
{
    free(env->tmp_envp);
    env->tmp_envp = new_env;
    env->counter = j;
}

int execute_unset(char **args, t_envp *env)
{
    int i;

    i = 0;
    while (args[i])
    {
        if (!is_valid_unset_key(args[i]))
        {
            printf("unset: '%s': not a valid identifier\n", args[i]);
            continue;
        }
        remove_env_var(env, args[i]);
        i++;
    }
    return (0);
}