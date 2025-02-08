import matplotlib.pyplot as plt
import matplotlib.animation as animation

def read_positions(filename):
    timesteps = []
    with open(filename, 'r') as f:
        current_timestep = []
        for line in f:
            if line.startswith('Timestep'):
                if current_timestep:
                    timesteps.append(current_timestep)
                    current_timestep = []
            elif line.strip():
                x, y = map(float, line.split())
                current_timestep.append((x, y))
        if current_timestep:
            timesteps.append(current_timestep)
    return timesteps

def animate(frame):
    plt.clf()
    positions = timesteps[frame]
    x = [pos[0] for pos in positions]
    y = [pos[1] for pos in positions]
    plt.scatter(x, y, s=5)
    plt.xlim(0, 100)
    plt.ylim(0, 100)
    plt.title(f'Timestep {frame}')

timesteps = read_positions('a1/src/boid_positions.txt')

fig = plt.figure()
anim = animation.FuncAnimation(fig, animate, frames=len(timesteps), interval=0.1)
plt.show()