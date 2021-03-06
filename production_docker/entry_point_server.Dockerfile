FROM ubuntu:20.04

ENV SHELL /bin/bash

# RUN apt update && DEBIAN_FRONTEND=noninteractive apt install -y sudo binutils git

# RUN apt update && DEBIAN_FRONTEND=noninteractive apt install -y libgtk-3-dev

# RUN apt update && DEBIAN_FRONTEND=noninteractive apt install -y libgl1-mesa-glx libavdevice-dev

ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID

# Create the user
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME

RUN groupmod --gid $USER_GID $USERNAME \
    && usermod --uid $USER_UID --gid $USER_GID $USERNAME


WORKDIR /home/$USERNAME/workfiles
COPY "./entry_point_server" "./entry_point_server"
COPY "./media_converter" "./media_converter"
RUN chmod +x "entry_point_server"
RUN chmod +x "media_converter"
RUN chown -R $USER_UID:$USER_GID /home/$USERNAME
USER $USERNAME

# ENV COMMAND_PORT 8787
# EXPOSE ${COMMAND_PORT}

CMD [ "./entry_point_server" ]
