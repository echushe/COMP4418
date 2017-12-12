def v_func(mdp, step, state, cache):
    """
    The value function of a state
    """
    if step == 0:
        return 0.0

    next_val1 = cache[step - 1][state - 1][0]
    if next_val1 is None:
        next_val1 = v_func_act(mdp, step - 1, state, 'stay', cache)
    next_val2 = cache[step - 1][state - 1][1]
    if next_val2 is None:
        next_val2 = v_func_act(mdp, step - 1, state, 'leave', cache)

    return max(next_val1, next_val2)


def v_func_act(mdp, step, state, act, cache):
    """
    The value function of a state and an action
    """
    reward, result = mdp[state][act]
    next_val = reward
    sum = 0.0
    for prob, next_state in result:
        sum += prob * v_func(mdp, step, next_state, cache)

    next_val += sum * 0.6
    
    if act == 'stay':
        cache[step][state - 1][0] = next_val
    else:
        cache[step][state - 1][1] = next_val

    return next_val


if __name__ == "__main__":
    # mdp defines the Markov Decision Process
    mdp = {1:{'stay':(1.0, [(1, 1)]),  'leave':(0.0, [(1, 2)])},\
           2:{'stay':(0.0, [(1, 2)]),  'leave':(5.0, [(0.5, 1), (0.5, 3)])},\
           3:{'stay':(-2.0, [(1, 3)]), 'leave':(-2.0, [(1, 3)])}}

    # This cache is to help improve performance
    # The time complexity would be O(2^n) without this cache
    cache = [];
    for i in range(1000):
        states = []
        for j in range(3):
            actions = []
            for k in range(2):
                actions.append(None)
            states.append(actions)
        cache.append(states)
    # Print the table of value functions
    for state in range(1, 4):
        for step in range(4):
            print('V{0}(s{1}): {2}'.format(step, state, round(v_func(mdp, step, state, cache), 5)), end='\t')
            print('V{0}(s{1}, S): {2}'.format(step, state, round(v_func_act(mdp, step, state, 'stay', cache), 5)), end='\t')
            print('V{0}(s{1}, L): {2}'.format(step, state, round(v_func_act(mdp, step, state, 'leave', cache), 5)), end='\t')
        print()
    # Check the value function after many steps
    for state in range(1, 4):
        print('V{0}(s{1}): {2}'.format(500, state, v_func(mdp, 500, state, cache)))

