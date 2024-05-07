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
        self.assertEqual(response.text, "Unauthorized")

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
        self.assertEqual(response.status_code, 422)
    
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
        student_ids_to_delete = [25, 26, 27]
        response = requests.delete(
            f"{self.base_url}/delete-selected",
            auth=self.correct_credentials,
            json=student_ids_to_delete
        )
        self.assertEqual(response.status_code, 200)

    def test_incorrect_delete_selected_students(self):
        student_ids_to_delete = [-1, -2, -3]
        response = requests.delete(
            f"{self.base_url}/delete-selected",
            auth=self.correct_credentials,
            json=student_ids_to_delete
        )
        self.assertEqual(response.status_code, 404)

if __name__ == "__main__":
    unittest.main()