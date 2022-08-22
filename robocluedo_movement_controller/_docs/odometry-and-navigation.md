
# Messages -- Navigation and Odometry

---

```{toctree}
---
maxdepth: 3
caption: Contents
---
./odometry-and-navigation.md
```

---

## Odometry message

**complete reference** : `nav_msgs/Odometry`

here's the content of the message:

```bash
# This represents an estimate of a position and velocity in free space.  
# The pose in this message should be specified in the coordinate frame given by header.frame_id.
# The twist in this message should be specified in the coordinate frame given by the child_frame_id

Header header
string child_frame_id
geometry_msgs/PoseWithCovariance pose
geometry_msgs/TwistWithCovariance twist
```

## TwistWithCovariance message

**complete reference** : `geometry_msgs/TwistWithCovariance`

message content:

```bash
# This expresses velocity in free space with uncertainty.
Twist twist

# Row-major representation of the 6x6 covariance matrix
# The orientation parameters use a fixed-axis representation.
# In order, the parameters are:
# (x, y, z, rotation about X axis, rotation about Y axis, rotation about Z axis)
float64[36] covariance
```

### Twist message

**complete reference** : `geometry_msgs/Twist`

```bash
# This expresses velocity in free space broken into its linear and angular parts.
Vector3  linear
Vector3  angular
```

### Vector3 message

**complete reference** : `geometry_msgs/Vector3`

```bash
float64 x
float64 y
float64 z
```

## PoseWithCovariance

**complete reference** : `geometry_msgs/PoseWithCovariance.msg`

message content:

```bash
# This represents a pose in free space with uncertainty.
Pose pose

# Row-major representation of the 6x6 covariance matrix
# The orientation parameters use a fixed-axis representation.
# In order, the parameters are:
# (x, y, z, rotation about X axis, rotation about Y axis, rotation about Z axis)
float64[36] covariance

```


