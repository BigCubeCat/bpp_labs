from .TestSetup import TestSetup


class ClusterTesting:
    def __init__(self, data, processor=print):
        self.processor = processor
        self.setup = [TestSetup(tst) for tst in data]

    def run_test(self, test: TestSetup):
        self.processor(str(test))
        self.processor("sleep ")

    def run_all(self):
        for index, test in enumerate(self.setup):
            self.processor(f"echo \"{index}\"\n")
            self.run_test(test)
