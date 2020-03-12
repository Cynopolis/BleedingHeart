import pygame
import serial

def createText(font, text, position):
    text = font.render(text, True, (0,0,0), (255,255,255))
    textRect = text.get_rect()
    textRect.center = position
    return text, textRect

pressure, heart_rate = 0, 0
pygame.init()
width, height = 900, 900
screen = pygame.display.set_mode((width, height))
pygame.display.set_caption('Bleeding Heart Interface')
clock = pygame.time.Clock()
font = pygame.font.SysFont('', 32)
texts = []
buttons = []
positions = [(width*.25, height *.1), (width*.75,height*.1),
             (width*.25, height*.2), (width*.75, height*.2),
             (width*.2, height*.3), (width*.7, height*.3)]
texts.append(createText(font, 'Pressure (PSI)', positions[0]))
texts.append(createText(font, 'Heart Rate (BPM)', positions[1]))
texts.append(createText(font, '0', positions[2]))
texts.append(createText(font, '0', positions[3]))

def main():
    done = False
    while not done:
        screen.fill((255, 255, 255))
        for text in texts:
            screen.blit(text[0], text[1])
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                done = True
        pygame.display.update()
        clock.tick(60)

    pygame.quit()
    return

class Button:
    pos = [0,0]
    size = [0,0]
    def __init__(self, position, size, pygame, function, arguments):
        self.pygame = pygame
        self.pos = position
        self.size = size
        self.function = function
        self.arguments = arguments
        return

    def isClicked(self):
        x, y = self.pygame.get_pos()[0], self.pygame.get_pos()[1]
        if x >= self.pos[0] and x <= self.pos[0]+self.size[0]:
            if y >= self.pos[1] and y <= self.pos[1]+self.size[1]:
                if pygame.mouse.get_pressed()[0] == True:
                    self.function(self.arguments)
                    return True
        return False

if __name__ == "__main__":
    main()
