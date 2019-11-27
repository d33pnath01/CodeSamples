import pygame
import random
pygame.init()
import json

from RobotCharTest.Robot import Robot
from RobotCharTest.Head import Head
from RobotCharTest.Body import Body
from RobotCharTest.Gun import Gun
from RobotCharTest.Cowboy import Cowboy
from RobotCharTest.RobotGun import RobotGun
from RobotCharTest.Bullet import Bullet
from RobotCharTest.AmmoManager import AmmoManager
from RobotCharTest.AudioManager import AudioManager
from RobotCharTest.RewardManager import RewardManager
from RobotCharTest.ScoreBoard import ScoreBoard
from RobotCharTest.Drone import Drone
from RobotCharTest.Overheat import Overheat
from RobotCharTest.BossDrone import BossDrone


win_width=1600
win_height=900
win=pygame.display.set_mode((win_width,win_height))

win_noise = 'WinSound.wav'
lose_noise = 'LoseSound.wav'
music = 'Music.wav'

clock = pygame.time.Clock()
key=None

global will_restart
will_restart = False


# CONFIG PARAMETER
bullet_speed = 30
cowboy_gun_rot_speed = 13
robot_gun_rot_speed = 0
drone_speed = 3
bullet_damage = -1
ammo_capacity = 6
reloading_time = 20
reward_kill_needed = 3
reward_last_time = 10000
overheat_meter_position = (100, 100)
# CONFIG END

def read_config():
    path="parameters.json"
    with open(path, "r") as f:
        load_dict = json.load(f)
        print(load_dict)
    global bullet_speed
    bullet_speed=load_dict["bullet_speed"]
    global cowboy_gun_rot_speed
    cowboy_gun_rot_speed = load_dict["cowboy_gun_rot_speed"]
    global drone_speed
    drone_speed=load_dict["drone_speed"]

def detect_collision(drone_count):
    winning = False
    failing = False
    display_boss=False
    count_temp=drone_count

    for drone in drones:
        for bullet in cowboy_bullet:
            if pygame.Rect.colliderect(drone.rect, bullet.rect):
                if drone.damage(rm.get_damage(bullet_damage)):
                    AudioManager.play("WinSound.wav")
                    count_temp+=1
                    sb.add_score(1)
                    rm.add()
                bullet.reset()
                AudioManager.play("explosion.wav")
                pass
        if pygame.Rect.colliderect(drone.rect, cowboy.rect):
            failing = True
    display_boss = (drone_count <= 2)

    for bossDrone in bossDrones:
        if bossDrone.isActive:
            for bullet in cowboy_bullet:
                if pygame.Rect.colliderect(bossDrone.rect, bullet.rect):
                    if bossDrone.damage(rm.get_damage(bullet_damage)):
                        sb.add_score(2)
                        rm.add()
                        bossDrone.isActive=False
                    bullet.reset()
                    AudioManager.play("RobotSmash.wav")
                    pass
            if pygame.Rect.colliderect(bossDrone.rect, cowboy.rect):
                failing = True

    for bullet in cowboy_bullet:
        if pygame.Rect.colliderect(cowboy.rect, bullet.rect):
            failing = True
    # for bodypart in robot.bodyparts:
    #     for b in cowboy_bullet:
    #         if pygame.Rect.colliderect(b.rect, bodypart.rect):
    #             b.reset()
    #             robot.sound()
    #             if canDamage == 4:
    #                 winning = robot.decrease_health(bodypart)
    #                 print("collide")
    #             if winning:
    #                 pygame.mixer.music.load(win_noise)
    #                 pygame.mixer.music.play(loops=1, start=0)
    #                 win_screen = pygame.image.load('win.png').convert_alpha()
    #                 win.blit(win_screen, [0, 0])
    #                 print("win")
    #                 pygame.display.update()
    #                 pygame.time.delay(1000)
    #                 pass

    return winning,failing,display_boss,count_temp
    pass

def main():
    # RESET EVERYTHING HERE
    read_config()
    will_restart = False
    clock.tick(10)
    pygame.display.set_caption("Space Spinning Cowboy")
    bg1 = pygame.image.load("background_v1.png").convert_alpha()
    bg2 = pygame.image.load("alien_land_v1.png").convert_alpha()
    font = pygame.font.SysFont("Arial", 40)
    control_text_content = "SPACE to SHOOT              R to RESTART"
    control_text = font.render(control_text_content, True, (255, 255, 0))

    audio_track = pygame.mixer.Sound(music)
    audio_track.set_volume(1)
    pygame.mixer.Channel(2).play(pygame.mixer.Sound(audio_track))

    global drone_count
    drone_count = 0

    global drones
    drones = []
    drone_one = Drone(path="drone.png", position=(win_width + 100, win_height / 2 - 200))
    drone_two = Drone(path="drone.png", position=(win_width + 1000, win_height / 2 - 100))
    drone_three = Drone(path="drone.png", position=(win_width + 800, win_height / 2 + 100))
    drone_four = Drone(path="drone.png", position=(win_width + 2000, win_height / 2 + 200))
    drones.append(drone_one)
    drones.append(drone_two)
    drones.append(drone_three)
    drones.append(drone_four)
    for drone in drones:
        drone.velocity = (-1, 0)

    global bossDrones
    bossDrones = []
    for i in range(0, 4):
        boss_temp=BossDrone(path="robot_head_v1.png", position=(win_width+10,win_height/2))
        boss_temp.velocity=(-1, 0)
        bossDrones.append(boss_temp)
    global boss_index
    boss_index=0
    global display_boss
    display_boss = False

    # global robot
    # head=Head(path="robot_head_v1.png",position=(win_width-300,win_height-200))
    # body=Body(path="robot_body_v1.png",position=(win_width-300,win_height-160))
    # robot_gun=RobotGun(path="robot_cannon_v1.png",position=(win_width-270,win_height-160))
    # bodyparts=[]
    # bodyparts.append(head)
    # bodyparts.append(body)
    # bodyparts.append(robot_gun)
    # robot=Robot(bodyparts)
    # robot.draw(win)

    global overheat
    overheat = Overheat(overheat_meter_position)
    overheat.draw(win)

    global cowboy
    cowboy = Cowboy("cowboy_v1.png", (200, win_height/2-200))
    cowboy_hand = pygame.image.load("cowboy_hand_v1.png").convert_alpha()
    cowboy.draw(win)

    global cowboy_gun
    gun_image_path = "gun_v1.png"
    cowboy_gun_position = (cowboy.rect.topleft[0]+215, cowboy.rect.topleft[1]+80)
    cowboy_gun = Gun(gun_image_path, cowboy_gun_position)
    cowboy_gun.draw(win)

    global ammo_manager
    ammo_manager = AmmoManager(ammo_capacity, reloading_time)

    global rm
    rm = RewardManager(reward_kill_needed, reward_last_time, (win_width / 2, win_height - 100))

    global cowboy_bullet
    bullet_image_path = "robot_bullet_v1.png"
    cowboy_bullet = []
    counter = ammo_manager.ammo_capacity

    global sb
    sb = ScoreBoard((100, 100))

    while counter > 0:
        b = Bullet(bullet_image_path, (cowboy.rect.topleft[0]+250, cowboy.rect.topleft[1]+40))
        cowboy_bullet.append(b)
        counter -= 1

    # global robot_bullet
    # robot_bullet=Bullet(bullet_image_path, position=(win_width-270,win_height-120))
    # robot_bullet.draw(win)

    cowboy_gun_angle = random.randint(0, 360)

    run = True
    # RESET END
    while run:
        shot_time = 0
        current_time = 0
        for event in pygame.event.get():
            if ((event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE) or \
                   (event.type == pygame.QUIT)):
                run = False
            elif event.type == pygame.KEYDOWN and event.key == pygame.K_SPACE and overheat.isFull is False:
                shot_time = pygame.time.get_ticks()

                #Increase Heat Meter for First Shot (Everytime)
                if(overheat.heatMeter == 0):
                    overheat.lastShot = shot_time

                if ammo_manager.try_shoot():
                    cowboy_bullet[ammo_manager.get_ammo_index()].shoot(cowboy_gun_angle)

                # Increasing or decreasing the overheat meter according to the shot frequency
                if shot_time - overheat.lastShot <= overheat.cooldown_between_bullets:
                    overheat.check_heat(increase=True, shot_time=shot_time)
            elif event.type == pygame.KEYDOWN and event.key == pygame.K_r:
                will_restart = True
                run = False

        #if overheat.isFull is True:
            #overheat.check_heat(increase=False, shot_time=shot_time)
        current_time = pygame.time.get_ticks()
        if current_time - overheat.lastShot > overheat.cooldown_between_bullets:
            print(overheat.isFull)
            overheat.check_heat(increase=False, shot_time=current_time)

        ammo_manager.update_reload(50)
        rm.update(50)

        win.blit(bg1, (0, 0))
        win.blit(bg2, (0, 0))
        win.blit(control_text, (win_width / 2 - control_text.get_width() / 2, 0))

        winning, failing,display_boss,drone_count = detect_collision(drone_count)

        # Boss
        if display_boss:
            for bossDrone in bossDrones:
                if not bossDrone.isActive:
                    bossDrones[boss_index].isActive=True
                    break
            drone_count = 0
            display_boss=False
        # Draw
        if not (winning or failing):
            sb.draw(win)
            rm.draw(win)
            cowboy.draw(win)
            overheat.draw(win)
            win.blit(cowboy_hand, (cowboy.rect.topleft[0] + 130, cowboy.rect.topleft[1] + 60))
            cowboy_gun_angle += cowboy_gun_rot_speed
            cowboy_gun.rotate(win, cowboy_gun_angle)
            # Drone
            for drone in drones:
                if drone.isActive:
                    drone.update_position(drone_speed)
                    drone.draw(win)
            for bossDrone in bossDrones:
                if bossDrone.isActive:
                    bossDrone.update_position(drone_speed)
                    bossDrone.draw(win)
            # Cowboy Bullet
            for b in cowboy_bullet:
                b.update_position(bullet_speed)
                if b.visibility:
                    b.draw(win)
        elif failing:
            cowboy.lose_sound()
            pygame.mixer.music.load(lose_noise)
            pygame.mixer.music.play(loops=0, start=0)
            lose_screen = pygame.image.load('lose.png').convert_alpha()
            win.blit(lose_screen, [0, 0])
        else:
            pass

        pygame.display.update()
        # if not robot_gun.rotate(robot_gun_rot_speed, 320):
        #     robot_bullet.velocity=(-1, 0) * bullet_speed
        #     robot_bullet.update_position(bullet_speed)
        #     pygame.mixer.music.load("RobotLaserSound.wav")
        #     pygame.mixer.music.play(loops=1, start=0)
        #     robot_bullet.draw(win)

    if will_restart:
        main()
    pass

main()
