import unittest
import requests
import json

class TestAPI(unittest.TestCase):
    def setUp(self):
        self.base_url = "http://127.0.0.1:8000"
        self.correct_credentials = ("pavel", "popov")
        
    def test_correct_auth(self):
        response = requests.post(f"{self.base_url}/auth", auth=self.correct_credentials)
        self.assertEqual(response.status_code, 200)
        self.assertEqual(response.text, "Ok")

    def test_incorrect_auth(self):
        incorrect_credentials = ("lol", "kek")
        response = requests.post(f"{self.base_url}/auth", auth=incorrect_credentials)
        self.assertEqual(response.status_code, 403)
        self.assertEqual(response.json()["detail"], "Unauthorized")

    def test_get_fields(self):
        response = requests.get(f"{self.base_url}/fields", auth=self.correct_credentials)
        self.assertEqual(response.status_code, 200)
        json_response = response.json()
        self.assertIn("total", json_response)
        self.assertIn("alphabetic", json_response)
        self.assertIn("comparable", json_response)
        self.assertIsInstance(json_response["total"], list)
        self.assertIsInstance(json_response["alphabetic"], list)
        self.assertIsInstance(json_response["comparable"], list)
        self.assertEqual(json_response["alphabetic"], ["last_name", "first_name", "patronymic", "group"])
        self.assertEqual(json_response["comparable"], ["year", "course"])

    def test_correct_add(self):
        correct_student_data = {
            "last_name": "Popov",
            "first_name": "Pavel",
            "patronymic": "Sergeevich",
            "group" : "AMI",
            "year" : 2020,
            "course" : 4,
            "photo" : "base64-string-with-photo"
        }

        response = requests.post(
            f"{self.base_url}/add",
            auth=self.correct_credentials,
            data=json.dumps(correct_student_data)
        )
        self.assertEqual(response.status_code, 200)
        json_response = response.json()
        self.assertEqual(json_response["last_name"], correct_student_data["last_name"])
        self.assertEqual(json_response["first_name"], correct_student_data["first_name"])
        self.assertEqual(json_response["patronymic"], correct_student_data["patronymic"])
        self.assertEqual(json_response["group"], correct_student_data["group"])
        self.assertEqual(json_response["year"], correct_student_data["year"])
        self.assertEqual(json_response["course"], correct_student_data["course"])
        self.assertEqual(json_response["photo"], correct_student_data["photo"])

    def test_incorrect_add(self):
        incorrect_student_data = {
            "first_name": 0,
            "patronymic": "Sergeevich",
            "group" : "AMI",
            "year" : 2020,
            "course" : 4,
            "photo" : "base64-string-with-photo"
        }
        response = requests.post(
            f"{self.base_url}/add",
            auth=self.correct_credentials,
            data=json.dumps(incorrect_student_data)
        )
        self.assertEqual(response.status_code, 500)
    
    def test_get_all_students(self):
        response = requests.get(f"{self.base_url}/students", auth=self.correct_credentials)
        self.assertEqual(response.status_code, 200)
        json_response = response.json()
        self.assertIsInstance(json_response, list)
        for student in json_response:
            self.assertIn("id", student)
            self.assertIn("last_name", student)
            self.assertIn("first_name", student)
            self.assertIn("patronymic", student)
            self.assertIn("year", student)
            self.assertIn("course", student)
            self.assertIn("group", student)
            self.assertIn("photo", student)

    def test_correct_delete_selected_students(self):
        response = requests.delete(f"{self.base_url}/delete-all", auth=self.correct_credentials)
        self.assertEqual(response.status_code, 200)
        to_delete_ids = []
        correct_student_data = [
            {   #0
                "last_name": "Popov",
                "first_name": "Pavel",
                "patronymic": "Sergeevich",
                "group" : "AMI",
                "year" : 2020,
                "course" : 2,
                "photo" : "base64-string-with-photo"
            },
            {   #1
                "last_name": "Petrov",
                "first_name": "Viktor",
                "patronymic": "Sergeevich",
                "group" : "AMI",
                "year" : 2020,
                "course" : 2,
                "photo" : "base64-string-with-photo"
            }]
        
        for student in correct_student_data:
                response = requests.post(
                    f"{self.base_url}/add",
                    auth=self.correct_credentials,
                    data=json.dumps(student)
                )
                self.assertEqual(response.status_code, 200)
                id_response = requests.post(
                    f"{self.base_url}/test-id",
                    auth=self.correct_credentials,
                    data=json.dumps(student)
                )
                self.assertEqual(id_response.status_code, 200)
                to_delete_ids+=id_response.json()

        response = requests.delete(
            f"{self.base_url}/delete-selected",
            auth=self.correct_credentials,
            json=to_delete_ids
        )
        self.assertEqual(response.status_code, 200)

        #check if students were correctly deleted
        after_deletion_ids = []
        for student in correct_student_data:
            id_response = requests.post(
                    f"{self.base_url}/test-id",
                    auth=self.correct_credentials,
                    data=json.dumps(student)
                )
            self.assertEqual(id_response.status_code, 200)
            after_deletion_ids+=id_response.json()
        
        self.assertEqual(after_deletion_ids, [-1 for _ in range(len(to_delete_ids))])

        response_after_delete = requests.get(f"{self.base_url}/students", auth=self.correct_credentials)
        num_students_after_delete = len(response_after_delete.json())
        self.assertEqual(num_students_after_delete, 0)


    def test_incorrect_delete_selected_students(self):
        student_ids_to_delete = [-1, -2, -3]
        response = requests.delete(
            f"{self.base_url}/delete-selected",
            auth=self.correct_credentials,
            json=student_ids_to_delete
        )
        self.assertEqual(response.status_code, 500)

    def test_delete_all_students(self):
        response_before_delete = requests.get(f"{self.base_url}/students", auth=self.correct_credentials)
        num_students_before_delete = len(response_before_delete.json())
        if num_students_before_delete:
            response = requests.delete(f"{self.base_url}/delete-all", auth=self.correct_credentials)
            self.assertEqual(response.status_code, 200)

            response_after_delete = requests.get(f"{self.base_url}/students", auth=self.correct_credentials)
            num_students_after_delete = len(response_after_delete.json())
            self.assertGreaterEqual(num_students_before_delete, 0)
            self.assertEqual(num_students_after_delete, 0)
        else:
            response_after_delete = requests.get(f"{self.base_url}/students", auth=self.correct_credentials)
            num_students_after_delete = len(response_after_delete.json())
            self.assertEqual(num_students_before_delete, 0)
            self.assertEqual(num_students_after_delete, 0)

    def test_filter_by_alphabetic(self):
        ids = []
        filtered_ids = []
        correct_student_data = [
            
            {   #0
                "last_name": "Popov",
                "first_name": "Pavel",
                "patronymic": "Sergeevich",
                "group" : "AMI",
                "year" : 2020,
                "course" : 2,
                "photo" : "base64-string-with-photo"
            },
            {   #1
                "last_name": "Petrov",
                "first_name": "Viktor",
                "patronymic": "Sergeevich",
                "group" : "AMI",
                "year" : 2020,
                "course" : 2,
                "photo" : "base64-string-with-photo"
            },
            {   #2
                "last_name": "Not-P",
                "first_name": "Nikolai",
                "patronymic": "Dmitrievich",
                "group" : "AMI",
                "year" : 2020,
                "course" : 4,
                "photo" : "base64-string-with-photo"
            },
            {   #3
                "last_name": "Not-P",
                "first_name": "Nikolai",
                "patronymic": "Dmitrievich",
                "group" : "AMI",
                "year" : 2016,
                "course" : 4,
                "photo" : "base64-string-with-photo"
            },
            {   #4
                "last_name": "Not-P",
                "first_name": "Nikolai",
                "patronymic": "Svyatoslavovich",
                "group" : "AMI",
                "year" : 2016,
                "course" : 4,
                "photo" : "base64-string-with-photo"
            },
            {   #5
                "last_name": "Nikitov",
                "first_name": "Nikolai",
                "patronymic": "Dmitrievich",
                "group" : "AMI",
                "year" : 2018,
                "course" : 4,
                "photo" : "base64-string-with-photo"
            }]

        for params in [({"last_name": "P"}, [0, 1]),
                       ({"first_name": "V"}, [1]),
                       ({"patronymic":"S"}, [0, 1, 4]),
                       ({ "year": 2016}, [3, 4]), 
                       ({"course": 4}, [2, 3, 4, 5]),
                       ({"last_name": "N", "year": 2016},[3, 4]),
                       ({"patronymic": "S", "course" : 2}, [0, 1]),
                       ({"course": 2, "and_greater" :1}, [0,1,2,3,4,5]),
                       ({"year" : 2018, "and_less" : 1}, [3, 4, 5]),
                       ({"patronymic": "D", "year": 2016, "and_greater": 1}, [2, 3, 5])
                       ]:
            ids.clear()
            filtered_ids.clear()    
            response = requests.delete(f"{self.base_url}/delete-all", auth=self.correct_credentials)
            self.assertEqual(response.status_code, 200)            

            for student in correct_student_data:
                response = requests.post(
                    f"{self.base_url}/add",
                    auth=self.correct_credentials,
                    data=json.dumps(student)
                )
                self.assertEqual(response.status_code, 200)

            filter_response = requests.get(f"{self.base_url}/filter", params=params[0], auth=self.correct_credentials)
            self.assertEqual(filter_response.status_code, 200)
            filtered_ids += filter_response.json()
            for correct_ans_idx in  params[1]:
                id_response = requests.post(
                    f"{self.base_url}/test-id",
                    auth=self.correct_credentials,
                    data=json.dumps(correct_student_data[correct_ans_idx])
                )
                self.assertEqual(id_response.status_code, 200)
                ids+=id_response.json()
            self.assertEqual(sorted(ids), sorted(filtered_ids))

if __name__ == "__main__":
    unittest.main()