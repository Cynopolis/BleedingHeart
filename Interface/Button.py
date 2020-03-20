import pygame
class Button:
    pos = [0,0]
    size = [0,0]
    color = (0, 80, 80)
    def __init__(self, position, size, screen, function, argument):
        self.pos = position
        self.size = size
        self.function = function
        self.argument = argument
        self.screen = screen
        return

    def isClicked(self, mouse_x, mouse_y, mouse_state):
        x, y = mouse_x, mouse_y
        if x >= self.pos[0] and x <= self.pos[0]+self.size[0]:
            if y >= self.pos[1] and y <= self.pos[1]+self.size[1]:
                if mouse_state == True:
                    self.function(self.argument)
                    return True
        return False

    def setPosition(self, x, y):
        self.pos[0], self.pos[1] = x, y
        return

    def setArgument(self, argument):
        self.argument = argument
        return

    def drawButton(self):
        pygame.draw.rect(self.screen, self.color,
                         pygame.Rect(self.pos[0], self.pos[1], self.size[0], self.size[1]))
        return