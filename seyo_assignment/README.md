This package was created with ROS Melodic and Gazebo 9.0 and includes a two wheeled robot model that can navigate the simulated blue path in Seyo.world.

The robot model is controlled and simulated with the following ROS nodes:
1) /sim_model
2) /robot_controller
3) /rotation_calcs
4) /gazebo
5) /robot_state_publisher

To run the project:
1. Ensure that all the nodes have the required exacutable permission using the "chmod +x" command.
2. Run the launch file "roslaunch seyo_assignment seyo_assignment.launch" to start all the nodes and to position the robot model at the starting position within seyo.world.
3. In a new terminal, the following commands can be issued in order to:
- Drive the robot model forward by a specified distance in metres: "rostopic pub -1 /userCommands seyo_assignment/user_commands fwd 1.0" (this will drive the robot foward by 1 metre)
- Turn the robot model 90 degrees left: "rostopic pub -1 /userCommands seyo_assignment/user_commands left 0" (the number is needed to satisfy the argument inputs but the value is ignored in the code)
- Turn the robot model 90 degrees right: "rostopic pub -1 /userCommands seyo_assignment/user_commands right 0" (the number is needed to satisfy the argument inputs but the value is ignored in the code)

Note that the first terminal, that was used to run the launch file, will indicate when the next required control command can be issued in the second terminal.

To control the robot model from start to finish; the required commands will need to be issued in the following order: (each command is issued for 3 seconds)
1) rostopic pub -1 /userCommands seyo_assignment/user_commands fwd 3.0
2) rostopic pub -1 /userCommands seyo_assignment/user_commands left 0
3) rostopic pub -1 /userCommands seyo_assignment/user_commands fwd 2.0
4) rostopic pub -1 /userCommands seyo_assignment/user_commands left 0
5) rostopic pub -1 /userCommands seyo_assignment/user_commands fwd 3.0
6) rostopic pub -1 /userCommands seyo_assignment/user_commands right 0
7) rostopic pub -1 /userCommands seyo_assignment/user_commands fwd 2.0
8) rostopic pub -1 /userCommands seyo_assignment/user_commands right 0
9) rostopic pub -1 /userCommands seyo_assignment/user_commands fwd 5.0
10) rostopic pub -1 /userCommands seyo_assignment/user_commands right 0
11) rostopic pub -1 /userCommands seyo_assignment/user_commands fwd 4.0
12) rostopic pub -1 /userCommands seyo_assignment/user_commands left 0
13) rostopic pub -1 /userCommands seyo_assignment/user_commands fwd 8.0
14) rostopic pub -1 /userCommands seyo_assignment/user_commands left 0
15) rostopic pub -1 /userCommands seyo_assignment/user_commands fwd 4.0
16) rostopic pub -1 /userCommands seyo_assignment/user_commands right 0
17) rostopic pub -1 /userCommands seyo_assignment/user_commands fwd 4.0
18) rostopic pub -1 /userCommands seyo_assignment/user_commands right 0
19) rostopic pub -1 /userCommands seyo_assignment/user_commands fwd 4.0
20) rostopic pub -1 /userCommands seyo_assignment/user_commands right 0
21) rostopic pub -1 /userCommands seyo_assignment/user_commands fwd 2.5
