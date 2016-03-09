README for Amazing Project:

In order to run Amazing Project from terminal, it requires three additional inputs.

Inputs: amazing [NUM_AVATARS] [DIFFICULTY] [HOST_IP]

NUM_AVATARS: number of avatars can range from 1 to 10
             ** maze does not print to stdout if only 1 avatar

DIFFICULTY: level of difficulty can range from 0 to 9

HOST_IP: host IP must be ID of flume.cs.dartmouth.edu, or 129.170.214.115

If the host is put in incorrectly, our program will display an error to the user.
The server will tell us if the other two arguments are invalid and initialization
will fail. Other errors from the server are caught in AMClient based upon the
message type sent from the server and are dealt with accordingly.

The testing files for this project were written to the testing directory and are
distinguished by nAvatars and difficulty in the format:

    testing_log_nAvatars_2_Difficulty_0

...for a maze with 2 avatars and difficulty 0.
