/*
*	Do Not Amend this section.
*
*/


//includes
//c headers
#include <stdio.h>
#include <stdlib.h>
//cpp headers
#include <cstdarg>
#include <string>
#include <iostream>
#include <fstream>
#include <streambuf>
//firebase headers
#include "FireDrive.h"
#include "firebase/app.h"
#include "firebase/auth.h"
#include "firebase/future.h"


namespace _FireDrive_ {

	//using the firebase namespace
	using namespace firebase;


//internal defines
#define AO_V2A(x) ((AppOptions*)x) //caster for void pointer to AppOptions Pointer
#define AP_V2A(x) ((App*)x) //caster for void pointer to App Pointer
#define US_V2A(x) ((firebase::auth::User*)x) //caster for void pointer to firebase::auth::User pointer, to convert the result returned by a callback
#define CDEL(x) if(x != NULL)delete x //deletes the x if it exists




//---------------------------------------------------CallbackParams---------------------------------------------------------

	_FireDrive_::CallbackParams::CallbackParams() : _data(NULL), _deleter(NULL), _funcID(FD_CB_ID::FID_UNKNOWN_FUNC) {}

	_FireDrive_::CallbackParams::CallbackParams(FD_CB_ID funcID, void* data, int sizeOfData, std::string dataDesc, _DataDeleter deleter)
		: _funcID(funcID), _data(NULL), _dataDesc(dataDesc), _deleter(NULL)
	{
		//deep copy - may be
		if (data) {
			if (sizeOfData < 1 || !deleter) {
				return;
			}

			//allocate the memory and copy
			_data = (void *)new char[sizeOfData];
			memcpy_s(_data, sizeOfData, data, sizeOfData);

			//set the deleter
			if (deleter)
				_deleter = deleter;
			else
				_deleter = [](void* mem) -> void {
					delete mem;
				};
		}
	}

	_FireDrive_::CallbackParams::~CallbackParams()
	{
		//free the reserved memory
		if (_data) {
			_deleter(_data);
		}
	}


//---------------------------------------------------FIREDRIVE---------------------------------------------------------------

	FireDrive::FireDrive(const char* pathToConfigJson) {
		//read file data
		unsigned int len = 0;
		fileData = readWholeFile(pathToConfigJson, &len);
		if (fileData->length() <= 10) {
			//error in file
			std::cerr << "Error Reading file." << std::endl;
			return;
		}

		//create the appOptions object
		appOptions = (void*)new AppOptions();

		//load from the file data into appOptions
		//AppOptions::LoadFromJsonConfig(fileData->c_str(), &appOpts);
		AppOptions::LoadFromJsonConfig(fileData->c_str(), (AppOptions*)appOptions);
		if (AO_V2A(appOptions)->api_key() == NULL || AO_V2A(appOptions)->api_key() == "") {
			//error in file
			std::cerr << "Error loading from json data!" << std::endl;
			return;
		}

		//create the App instance
		app = (void*)App::Create(*AO_V2A(appOptions), "Harleys");
		if (!AP_V2A(app)) {
			//error creating the app object
			std::cerr << "Error creating the app object!" << std::endl;
			return;
		}
	}

	FireDrive::FireDrive(const char* jsonConfigContent, size_t len)
	{
		//store the json data
		std::string str = jsonConfigContent;
		fileData = std::make_shared<std::string>(str);
		if (fileData->length() <= 10) {
			//error in file
			std::cerr << "Error Reading file." << std::endl;
			return;
		}

		//create the appOptions object
		appOptions = (void*)new AppOptions();

		//load from the file data into appOptions
		AppOptions::LoadFromJsonConfig(fileData->c_str(), (AppOptions*)appOptions);
		if (AO_V2A(appOptions)->api_key() == NULL || AO_V2A(appOptions)->api_key() == "") {
			//error in file
			std::cerr << "Error loading from json data!" << std::endl;
			return;
		}

		//create the App instance
		app = (void*)App::Create(*AO_V2A(appOptions), "Harleys");
		if (!AP_V2A(app)) {
			//error creating the app object
			std::cerr << "Error creating the app object!" << std::endl;
			return;
		}
	}

	FireDrive::FireDrive(const FireDrive& obj) {}

	void FireDrive::operator=(const FireDrive& obj) {}

	FireDrive::~FireDrive() {
		if (IsSignedIn())
			LogOut();

		//delete the auth, appOptions AND APP
		auth::Auth* _auth = auth::Auth::GetAuth(AP_V2A(app));
		CDEL(_auth);
		CDEL(AP_V2A(app));
		CDEL(AO_V2A(appOptions));
		//CDEL(US_V2A(user)); //as we are returning or storing the ref to the original, we dont need to delete it
	}

	std::shared_ptr<const std::string> FireDrive::getJsonConfigFileData() {
		return fileData;
	}

	bool FireDrive::CreateUser(const char* email, const char* password, FireDriveException& exception, _OnCompletionCallBack callback) {

		//get Auth class object
		auth::Auth* _auth = auth::Auth::GetAuth(AP_V2A(app));

		//pointer to the user pointer
		volatile void** userPointer = &user;
		volatile bool* _isSignedIn = &FD_is_Signed_in;

		//declare lamda function for the call back - only registers when pending
		//callback - is the function to be called at the end, which was provided by the user
		//exception - is the exception object, which is to contain the exception details if any
		//userPointer - pointer to the user pointer to be initialized with the result user details
		auto callBFunc = [callback, userPointer, _isSignedIn](const Future<auth::User*>& _result) -> void {
			std::string customessage = "";
			bool res = false;

			if (_result.status() == kFutureStatusComplete) {
				//check now
				if (_result.error() == auth::kAuthErrorNone) {
					//success
					res = true;

					//store the user details in the local memory
					firebase::auth::User* _user = *_result.result();
					*userPointer = (void*)_user;
					*_isSignedIn = true;
				}
				else {
					//error
					res = false;
					customessage.append(_result.error_message());
				}
			}
			else {
				//should only enter this function if the operation is complete
				//hence error!!!!
				res = false;
				customessage.append("Callback called OUT OF ORDINARY!!!");
			}

			//call the callback provided by user if not NULL
			if (callback) {
				//Failing here
				CallbackParams _params(FID_CREATE_USER);
				std::shared_ptr<CallbackParams> params = std::make_shared<CallbackParams>(_params);

				if (res == false) {
					FireDriveException fde(FireDriveExceptions::UNKNOWN_EXCEPTION, "");
					callback(
						res,
						fde,
						params
					);
				}
				else {
					FireDriveException fde(FireDriveExceptions::NO_EXCEPTION, "");
					callback(
						res,
						fde,
						params
					);
				}
			}
		};

		//create the user
		if (!IsSignedIn()) {
			Future<auth::User*> result = _auth->CreateUserWithEmailAndPassword(email, password);

			//resgister the call back function
			result.OnCompletion(callBFunc);

			//pre check if failed
			if (result.status() == kFutureStatusComplete || result.status() == kFutureStatusInvalid) {
				if (result.error() == auth::kAuthErrorNone) {
					return true;
				}
				else {
					//already failed for some reason
					//handle exception
					FireDriveException fde(UNKNOWN_EXCEPTION, result.error_message());
					exception = fde;
					return false;
				}
			}
		}
		return true; //return true for pending
	}

	bool FireDrive::SignInUser(const char* email, const char* password, FireDriveException& exception, _OnCompletionCallBack callback) {
		
		//get Auth
		auth::Auth* _auth = auth::Auth::GetAuth(AP_V2A(app));

		//pointer to the user pointer, to be initialized with the result user pointer copy
		volatile void** userPointer = &user;
		volatile bool* _isSignedIn = &FD_is_Signed_in;

		//declare lamda function for the call back - only registers when pending
		auto callBFunc = [callback, &exception, userPointer, _isSignedIn](const Future<auth::User*>& _result) -> void {
			std::string customessage = "";
			bool res = false;

			if (_result.status() == kFutureStatusComplete) {
				//check now
				if (_result.error() == auth::kAuthErrorNone) {
					//success
					res = true;

					//HANDLE the operations here after----
					{
						//store the basic user details, retrieved from the result object
						//may be possible to minimize - TODO
						firebase::auth::User* _user = *_result.result();
						*userPointer = (void*)_user;
						*_isSignedIn = true;
					}

				}
				else {
					//error
					res = false;
					customessage.append(_result.error_message());
				}
			}
			else {
				//should only enter this function if the operation is complete
				//hence error!!!!
				res = false;
				customessage.append("Callback called OUT OF ORDINARY!!!");
			}

			//call the callback provided by user if not NULL
			if (callback) {
				//Failing here
				CallbackParams _params(FID_SIGNIN_USER);
				std::shared_ptr<CallbackParams> params = std::make_shared<CallbackParams>(_params);

				if (res == false) {
					FireDriveException fde(FireDriveExceptions::UNKNOWN_EXCEPTION, "");
					callback(
						res,
						fde,
						params
					);
				}
				else {
					FireDriveException fde(FireDriveExceptions::NO_EXCEPTION, "");
					callback(
						res,
						fde,
						params
					);
				}
			}
		};

		//sign in
		if (!IsSignedIn()) {
			Future<auth::User*> result = _auth->SignInWithEmailAndPassword(email, password);

			//resgister the call back function
			result.OnCompletion(callBFunc);

			//pre check and verify
			if (result.status() == kFutureStatusComplete || result.status() == kFutureStatusInvalid) {
				if (result.error() == auth::kAuthErrorNone) {
					return true;
				}
				else {
					//already failed for some reason
					//handle exception
					FireDriveException fde(UNKNOWN_EXCEPTION, result.error_message());
					exception = fde;
					return false;
				}
			}
		}

		return true;
	}

	bool FireDrive::IsSignedIn() const {
		if (user && FD_is_Signed_in) {
			if (US_V2A(user)->uid().empty() || US_V2A(user)->uid().length() < 10)
				return false;
			else
				return true;
		}
		return false;
	}

	bool FireDrive::LogOut() const {
		//directly return true if nothing is there already
		if (!IsSignedIn())
			return true;
		//get Auth
		auth::Auth* _auth = auth::Auth::GetAuth(AP_V2A(app));
		if (_auth) {
			_auth->SignOut();
			user = NULL;
		}
		else return false;
		return true;
	}

	std::shared_ptr<const std::string> FireDrive::GetCurrentUserUID() const {
		if (IsSignedIn()) {
			return std::make_shared<std::string>(US_V2A(user)->uid());
		}
		return NULL;
	}

	std::shared_ptr<const std::string> FireDrive::GetCurrentUserDispName() const {
		if (IsSignedIn()) {
			return std::make_shared<std::string>(US_V2A(user)->display_name());
		}
		return NULL;
	}

	bool FireDrive::ChangeUserDispName(const char* newDispName, FireDriveException& exception, _OnCompletionCallBack callback) {
		if (!IsSignedIn()) {
			FireDriveException fde(FireDriveExceptions::INVALID_OPERATION, "Error : User Not Signed In!!");
			exception = fde;
			return false;
		}

		//declare lamda function for the call back - only registers when pending
		auto callBFunc = [callback](const Future<void>& _result) -> void {
			std::string customessage = "";
			bool res = false;

			if (_result.status() == kFutureStatusComplete) {
				//check now
				if (_result.error() == auth::kAuthErrorNone) {
					//success
					res = true;

					//HANDLE the operations here after----
					{
						//reload the user data, if required
						
					}
				}
				else {
					//error
					res = false;
					customessage.append(_result.error_message());
				}
			}
			else {
				//should only enter this function if the operation is complete
				//hence error!!!!
				res = false;
				customessage.append("Callback called OUT OF ORDINARY!!!");
			}

			//call the callback provided by user if not NULL
			if (callback) {
				//Failing here
				CallbackParams _params(FID_CHANGE_USER_DISP_NAME);
				std::shared_ptr<CallbackParams> params = std::make_shared<CallbackParams>(_params);

				if (res == false) {
					FireDriveException fde(FireDriveExceptions::UNKNOWN_EXCEPTION, "");
					callback(
						res,
						fde,
						params
					);
				}
				else {
					FireDriveException fde(FireDriveExceptions::NO_EXCEPTION, "");
					callback(
						res,
						fde,
						params
					);
				}
			}
		};

		//update profile		
		
		//create the userprofile
		firebase::auth::User::UserProfile userProfile;
		userProfile.display_name = _strdup(newDispName);
		
		//update
		Future<void> result = US_V2A(user)->UpdateUserProfile(userProfile);
		
		//resgister the call back function
		result.OnCompletion(callBFunc);

		//pre check and verify
		if (result.status() == kFutureStatusComplete || result.status() == kFutureStatusInvalid) {
			if (result.error() == auth::kAuthErrorNone) {
				return true;
			}
			else {
				//already failed for some reason
				//handle exception
				FireDriveException fde(UNKNOWN_EXCEPTION, result.error_message());
				exception = fde;
				return false;
			}
		}

		return true;
	}

	bool _FireDrive_::FireDrive::IsEmailVerified() {
		if (!IsSignedIn())
			return false;
		//verify the email
		return (US_V2A(user)->is_email_verified()) ? true : false;
	}

	bool _FireDrive_::FireDrive::InitiateEmailAuth(FireDriveException& exception, _OnCompletionCallBack callback)
	{
		if (!IsSignedIn()) {
			FireDriveException fde(FireDriveExceptions::INVALID_OPERATION, "Error : User Not Signed In!!");
			exception = fde;
			return false;
		}


		//declare lamda function for the call back - only registers when pending
		auto callBFunc = [callback](const Future<void>& _result) -> void {
			std::string customessage = "";
			bool res = false;


			if (_result.status() == kFutureStatusComplete) {
				//check now
				if (_result.error() == auth::kAuthErrorNone) {
					//success
					res = true;

					//HANDLE the operations here after----
					{
						//reload the user data, if required

					}
				}
				else {
					//error
					res = false;
					customessage.append(_result.error_message());
				}
			}
			else {
				//should only enter this function if the operation is complete
				//hence error!!!!
				res = false;
				customessage.append("Callback called OUT OF ORDINARY!!!");
			}

			//call the callback provided by user if not NULL
			if (callback) {
				//Failing here
				CallbackParams _params(FID_INITIATE_EMAIL_AUTH);
				std::shared_ptr<CallbackParams> params = std::make_shared<CallbackParams>(_params);

				if (res == false) {
					FireDriveException fde(FireDriveExceptions::UNKNOWN_EXCEPTION, "");
					callback(
						res,
						fde,
						params
					);
				}
				else {
					FireDriveException fde(FireDriveExceptions::NO_EXCEPTION, "");
					callback(
						res,
						fde,
						params
					);
				}
			}
		};

		//send the authentication/verification code
		Future<void> result = US_V2A(user)->SendEmailVerification();
		//resgister the call back function
		result.OnCompletion(callBFunc);

		//pre check and verify
		if (result.status() == kFutureStatusComplete || result.status() == kFutureStatusInvalid) {
			if (result.error() == auth::kAuthErrorNone) {
				return true;
			}
			else {
				//already failed for some reason
				//handle exception
				FireDriveException fde(UNKNOWN_EXCEPTION, result.error_message());
				exception = fde;
				return false;
			}
		}

		return true;
	}



	//----------------------------------------------------FIREDRIVEEXCEPTION----------------------------------------------------

	FireDriveException::FireDriveException() : customMessage(""), fullMessage(""), message(""), exception(NO_EXCEPTION) {}

	FireDriveException::FireDriveException(FireDriveExceptions exception, const char* customMessage) :
		customMessage(""), fullMessage(""), message("")
	{
		//store the messages
		this->exception = exception;

		//store the message
		switch (this->exception) {
		case NO_EXCEPTION:
			break;
		case FILE_NOT_FOUND_EXCEPTION:
			message = "File Not Found. Please check the path to file.";
			break;
		case INVALID_ARGUMENT:
			message = "Invalid Arguments Passed.";
			break;
		case NO_MEMORY:
			message = "Fatal! No more free memory.";
			break;
		case INVALID_OPERATION:
			message = "Fatal! Operation not allowed.";
			break;
		default:
			message = "Fatal! Unknown Exception.";
			break;
		}

		//if custom message exists
		if (customMessage != NULL) {
			this->customMessage = customMessage;
			fullMessage = message + " : " + customMessage;
		}
	}

	//TO-DO
	FireDriveException::FireDriveException(const char* customMessageFormat, FireDriveExceptions exception, ...) :
		customMessage(""), fullMessage(""), message("")
	{
		//store the messages
		this->exception = exception;

		//store the message
		switch (this->exception) {
		case NO_EXCEPTION:
			break;
		case FILE_NOT_FOUND_EXCEPTION:
			message = "File Not Found. Please check the path to file.";
			break;
		case INVALID_ARGUMENT:
			message = "Invalid Arguments Passed.";
			break;
		case NO_MEMORY:
			message = "Fatal! No more free memory.";
			break;
		case INVALID_OPERATION:
			message = "Fatal! Operation not allowed.";
			break;
		default:
			message = "Fatal! Unknown Exception.";
			break;
		}

		//if custom message exists
		if (customMessageFormat != NULL || customMessageFormat != "") {
			//read the varargs and prepare the message string
			//va_list list;

			//int len = strlen(customMessage) + strlen(message) + 5;
			//this->customMessage = strdup(customMessage);
			//fullMessage = (char*)malloc(len); //create full message
			//sprintf_s(fullMessage, len, "%s : %s", message, customMessage);
		}
	}

	FireDriveException::FireDriveException(const FireDriveException& exception) :
		customMessage(""), fullMessage(""), message("")
	{
		this->customMessage = exception.customMessage;
		this->exception = exception.exception;
		this->fullMessage = exception.fullMessage;
		this->message = exception.message;
	}

	void FireDriveException::operator=(const FireDriveException& obj)
	{
		this->customMessage = obj.customMessage;
		this->exception = obj.exception;
		this->fullMessage = obj.fullMessage;
		this->message = obj.message;
	}

	FireDriveException::~FireDriveException()
	{
		//get rid of the memory
		/*free(message);
		if (customMessage) free(customMessage);
		if (fullMessage) free(fullMessage);*/
	}

	std::string FireDriveException::getMessage()
	{
		return message;
	}

	std::string FireDriveException::getCustomMessage()
	{
		return customMessage;
	}

	std::string FireDriveException::getFullMessage()
	{
		return customMessage;
	}

	



	//---------------------------------------------------FIREDRIVEUTILITY-------------------------------------------------------

	std::shared_ptr<std::string> FireDriveUtility::readWholeFile(const char* pathToFile, unsigned int* len)
	{
		//open and read the JSON CONFIG file
		try {
			//using string
			std::ifstream ifs;
			std::shared_ptr<std::string> str; //this might be null
			std::streampos length = 0;

			//open the file
			ifs.open(pathToFile);
			if (!ifs.is_open()) {
				//error
				throw FireDriveException(FireDriveExceptions::FILE_NOT_FOUND_EXCEPTION, "Not able to find the given file.");
			}

			//read the string into the stream
			ifs.seekg(0, std::ios::end); //seek end

			length = ifs.tellg(); //get the length

			//reserve the space needed
			str = std::make_shared<std::string>();
			str->reserve((int)length);
			if (str->capacity() < (size_t)length) {
				//error
				ifs.close();
				throw FireDriveException(FireDriveExceptions::NO_MEMORY, "Not able to reserve more memory!");
			}

			ifs.seekg(0, std::ios::beg); //reverse back to begining

			//assign h
			str->assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
			if (str->length() != length) {
				//error
				ifs.close();
				throw FireDriveException(FireDriveExceptions::UNKNOWN_EXCEPTION, "Unknown error occured while reading the file!");
			}

			//close the file and return the string pointer and len
			ifs.close();
			if (len) *len = (int)length;
			return str;
		}
		catch (FireDriveException exception) {
			//print the error for now
			switch (exception.exception) {
			case UNKNOWN_EXCEPTION:
			case INVALID_ARGUMENT:
			case NO_MEMORY:
			case FILE_NOT_FOUND_EXCEPTION:
				std::cerr << exception.fullMessage << std::endl;
				break;
			}

			return NULL;
		}
	}

	bool _FireDrive_::FireDriveUtility::FindDigits(const long input, const int checkDigits, int& foundDigits)
	{
		long n = input;
		foundDigits = 0;
		//count
		while (n >= 1) {
			n = n / 10;
			foundDigits++;
		}
		return (foundDigits == checkDigits) ? true : false;
	}
}