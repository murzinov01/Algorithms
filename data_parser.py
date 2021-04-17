import pandas as pd


class DataParser:
    def __init__(self, file_name="I1.txt"):
        self.file_name = file_name
        self.trucks = 0
        self.shops = 0
        self.truck_capacity = 0
        self.data = list()
        self.data_frame = None
        self._parse()
        self._create_data_frame()

    def get_trucks(self) -> int:
        return self.trucks

    def get_shops(self) -> int:
        return self.shops

    def get_truck_capacity(self) -> int:
        return self.truck_capacity

    def get_data(self) -> list:
        return self.data

    def get_data_frame(self) -> pd.DataFrame:
        return self.data_frame

    def get_params(self) -> dict:
        return {"trucks": self.trucks,
                "shops": self.shops,
                "truck_capacity": self.truck_capacity,
                "data": self.data_frame}

    def _parse(self) -> None:
        with open(self.file_name, 'r', encoding='utf-8') as file:
            first_line = file.readline()
            self.shops, self.trucks, self.truck_capacity = (int(number.strip(" ")) for number in first_line.split())
            for line in file:
                tokens = [int(number.strip(" ")) for number in line.split()]
                self.data.append(tokens)

    def _create_data_frame(self) -> None:
        self.data_frame = pd.DataFrame(data=self.data, columns=["Id", "X", "Y", "Demand", "TimeOpen", "TimeClose",
                                                                "Unloading"])
