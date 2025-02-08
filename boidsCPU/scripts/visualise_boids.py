import pygame
import sys
import math
import os

pygame.init()
screen_width, screen_height = 800, 600
screen = pygame.display.set_mode((screen_width, screen_height))
clock = pygame.time.Clock()

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

script_dir = os.path.dirname(os.path.abspath(__file__)) 
file_path = os.path.join(script_dir, '../src/boid_positions.txt')  # get absolute path to boid positions

timesteps = read_positions(file_path)

# determine the bounds of the environment based on particle positions
def get_bounds(timesteps):
    min_x = min_y = float('inf')
    max_x = max_y = float('-inf')

    for timestep in timesteps:
        for x, y in timestep:
            if x < min_x: min_x = x
            if x > max_x: max_x = x
            if y < min_y: min_y = y
            if y > max_y: max_y = y

    return min_x, max_x, min_y, max_y

min_x, max_x, min_y, max_y = 0, 100, 0, 100

# calculate scaling factors
def scale_position(x, y, min_x, max_x, min_y, max_y, screen_width, screen_height):
    scale_x = screen_width / (max_x - min_x)
    scale_y = screen_height / (max_y - min_y)
    return int((x - min_x) * scale_x), int((y - min_y) * scale_y)

def calculate_angle(prev_pos, current_pos):
    dx = current_pos[0] - prev_pos[0]
    dy = current_pos[1] - prev_pos[1]
    return math.atan2(-dy, dx)  # negative dy because pygame y-axis is inverted

def draw_triangle(surface, color, pos, angle, size):
    points = [
        (pos[0] + size * math.cos(angle), pos[1] - size * math.sin(angle)),
        (pos[0] + size * math.cos(angle + 2.6), pos[1] - size * math.sin(angle + 2.6)),
        (pos[0] + size * math.cos(angle - 2.6), pos[1] - size * math.sin(angle - 2.6))
    ]
    pygame.draw.polygon(surface, color, points)

# initialize angles for each boid
boid_angles = [0] * len(timesteps[0])
angle_smoothing_factor = 0.4  # adjust this value to change smoothing (0.1 to 0.5 works well)
min_movement_threshold = 0.1  # minimum movement required to update angle

running = True
frame = 0
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit()

    screen.fill((255, 255, 255))  # white background

    current_positions = timesteps[frame]
    prev_positions = timesteps[frame - 1] if frame > 0 else current_positions

    for i, (prev_pos, current_pos) in enumerate(zip(prev_positions, current_positions)):
        scaled_prev_x, scaled_prev_y = scale_position(prev_pos[0], prev_pos[1], min_x, max_x, min_y, max_y, screen_width, screen_height)
        scaled_x, scaled_y = scale_position(current_pos[0], current_pos[1], min_x, max_x, min_y, max_y, screen_width, screen_height)
        
        movement = math.sqrt((scaled_x - scaled_prev_x)**2 + (scaled_y - scaled_prev_y)**2)
        
        if movement > min_movement_threshold:
            target_angle = calculate_angle(prev_pos, current_pos)
            angle_diff = (target_angle - boid_angles[i] + math.pi) % (2 * math.pi) - math.pi
            boid_angles[i] += angle_diff * angle_smoothing_factor
        
        draw_triangle(screen, (0, 0, 255), (scaled_x, scaled_y), boid_angles[i], 3)  # blue triangles

    pygame.display.flip()
    clock.tick(30)  # 30 FPS
    frame = (frame + 1) % len(timesteps)