# message 'ActionFeedback'

## === about the action sending the feedback message

# feedback from action
string action_name

# with parameters
diagnostic_msgs/KeyValue[] parameters

## === plan statistics

# goal achieved, problem solved
bool goal_achieved

# if a failure occurred
bool failure

# specific for hardware failure
bool hw_failure

# failure of the navigation system
bool failure_nav_system

# failure of the manipulator
bool failure_manipulation

# details about the response as string
string details
