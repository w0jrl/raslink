# RasLink-Testing
This source code is bleeding-edge, and should not be used by persons who are not used to debugging their own systems!
These files are used to debug new features, as well as test new versions of the Raspberry Pi and Debian.
#So, you think you're brave?
Ok... I warned you!
To switch to the testing version, do the following:

1. cd /usr/src/utils

2. git checkout testing

3. system-update

After the update finishes, you'll be on the bleeding-edge!
Please report bugs over at:

https://jlappliedtechnologies.com/groups/all-star/

When you file your bug, please mention that you are using the testing branch. Bugs in the master branch that are critical will always get
fixed first, but the testing branch will always have the latest toys!
# Help! Something's broken, and I want/need to switch back to the stable version!
No problem!
To switch back to the stable version, do the following:

1. cd /usr/src/utils

2. git checkout master

3. system-update

After the update finishes, you'll be back on the stable version.

