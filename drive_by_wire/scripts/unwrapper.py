import math

class Unwrapper:
    def __init__(self, thresh):
        self.thresh = thresh
        self.prev = 0

    def unwrap(self, val):
        num = 0
        if val - self.prev < -1*self.thresh:
            num = val + 2*math.pi
        elif val - self.prev > self.thresh:            
            num = val - 2*math.pi
        else:
            num = val
        self.prev = num
        return num            