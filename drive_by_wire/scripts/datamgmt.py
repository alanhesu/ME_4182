class CartData:
	def __init__(self, categories, samples = 1000):
		self.data = dict()
		self.time = dict()
		for catname in categories:
			self.data[catname] = [0]
			self.time[catname] = [0]
		self.samples = samples
		

	def reset(self):
		self.data = dict()
		self.samples = 1000
		self.time = dict()

	def setSampleLimit(self, limit):
		self.samples = limit
		for key in self.data:
			currnumsamples = len(self.data[key])
			if currnumsamples > self.samples:
				diff = currnumsamples - self.samples
				self.data[key] = self.data[key][diff:]
				self.time[key] = self.time[key][diff:]

	def addTime(self, category, tpoint):
		currnumsamples = len(self.time[category])

		if currnumsamples == self.samples:
			self.time[category] = self.time[category][1:] + [tpoint]
		elif currnumsamples > self.samples:
			diff = currnumsamples - self.samples
			self.time[category] = self.time[category][diff+1:] + [tpoint]
		else:
			self.time[category].append(tpoint)

	def addData(self, category, datapoint):
		currnumsamples = len(self.data[category])

		if currnumsamples == self.samples:
			self.data[category] = (self.data[category])[1:] + [datapoint]
		elif currnumsamples > self.samples:
			diff = currnumsamples - self.samples
			self.data[category] = (self.data[category])[diff+1:] + [datapoint]
		else:
			self.data[category].append(datapoint)

	def getCategory(self, category):
		return self.data[category]

	def getData(self):
		return self.data

	def getTime(self, category):
		return self.time[category]