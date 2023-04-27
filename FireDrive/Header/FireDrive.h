#ifndef FIRE_BASE_DRIVE_HEADER
#define FIRE_BASE_DRIVE_HEADER

//includes
#include <string>
#include <memory>


namespace _FireDrive_ {

	//class prototypes
	class CallbackParams;
	class FireDriveException;
	class FireDriveUtility;
	class FireDriveAuthentication;
	class FireDriveUser;
	class FireDrive;




	//function prototypes 
	//template<class t>
	//using _OnCompletionCallBack = void (*) (bool, FireDriveException, CallbackParams<t>);
	//template <class t>
	


	//function pointer prototypes
	typedef void (*_OnCompletionCallBack) (bool, FireDriveException, std::shared_ptr<CallbackParams>);
	typedef void (*_DataDeleter) (void *);

	//FireDrive_CallBack_ID
	//enum containing the IDs of the functions, that provides a callback feature
	//can be used to switch between different functionalities, while a callback gets called,
	//by determining which function called it.
	//this is passed into the CallbackParams class during callaing a callback
	typedef enum _FD_CB_ID {
		FID_UNKNOWN_FUNC = 0,
		FID_CREATE_USER = 20101,
		FID_SIGNIN_USER,
		FID_INITIATE_EMAIL_AUTH,
		FID_CHANGE_USER_DISP_NAME
	}FD_CB_ID;


	////TODO
	////prototype class for the callbacks to be prorvided with data
	////the data send within this class will be a shared pointer and will stay alive until everyone is done with it
	class CallbackParams {
	private:
		_DataDeleter _deleter; //passed deleter or custom deleter for the data variable
		CallbackParams();

	public :
		FD_CB_ID _funcID;	//Function ID of the function that called this callback
		void* _data;	//any kind of data to be passed on to the user
		std::string _dataDesc; //text describing the passed data

		CallbackParams(FD_CB_ID funcID, void* data = NULL, int sizeOfData = 0, std::string dataDesc = "", _DataDeleter deleter = NULL);
		~CallbackParams();
	};


	typedef enum _FireDriveExceptions {
		UNKNOWN_EXCEPTION = -1,
		NO_EXCEPTION = 0,
		FILE_NOT_FOUND_EXCEPTION = 20021,
		INVALID_ARGUMENT,
		NO_MEMORY,
		INVALID_OPERATION
	}FireDriveExceptions, *PFireDriveExceptions;

	//class for FireDrive related exceptions
	class FireDriveException {

	public:
		FireDriveExceptions exception;
		std::string message;
		std::string customMessage;
		std::string fullMessage;

		FireDriveException();

		FireDriveException(FireDriveExceptions exception, const char* customMessage = NULL);

		FireDriveException(const char* customMessageFormat, FireDriveExceptions exception, ...);

		FireDriveException(const FireDriveException& exception);

		~FireDriveException();

		std::string getMessage();

		std::string getCustomMessage();

		std::string getFullMessage();

		void operator=(const FireDriveException& obj);
	};


	//class for FireDrive Utility Functions
	class FireDriveUtility {

	protected:

		//function to read the whole file provided the path of
		//remember to use free() to free the allocated buffer memory
		//len can be null
		//returns a shared pointer to the string
		std::shared_ptr<std::string> readWholeFile(const char* pathToFile, unsigned int* len = NULL);

		/*FindDigits Function
		* The function is used to check if the input number is having the Number of digits same to the checkDigits value.
		* 
		* Paramters -
		* input - the number which's number of digits are to be checked
		* checkDigits - this value should be matched with the checked count
		* foundDigits - a reference which will get the found number digits as output
		* 
		* Returns true if the count matched, false if not
		*/
		bool FindDigits(const long input, const int checkDigits, int& foundDigits);

	};


	//interface class for firebase authentication
	class FireDriveAuthentication {
	public:

		/*CreateUser Function
		* Creates a new User
		*
		* Parameters -
		* email = the email to be used to login
		* password = the password to be set
		* exception = refference to a FireDriveException object
		* callback = a callback to be called at the end of the operation completion. Can be NULL.
		*
		* Returns true if successful or false if not. Check the exception if false.
		*/
		virtual bool CreateUser(const char* email, const char* password, FireDriveException& exception, _OnCompletionCallBack callback) = 0;


		/*SignInUser Function
		* Signs a user in, if provided correct password and email
		*
		* Parameters -
		* email = the email to be used while logging in
		* password = the password to be used while logging in
		* exception = refference to a FireDriveException object
		* callback = a callback function to be called at the end of the operation completion. Can be NULL.
		* 
		* Returns true if function executed successfully, false if not.
		*/
		virtual bool SignInUser(const char* email, const char* password, FireDriveException& exception, _OnCompletionCallBack callback = NULL) = 0;


		/*LogOut Function
		* Logs a user out, if currently logged in
		*
		* Returns true if user found and logged out successfully and also when no user is logged in, false if not able to log out the user.
		*/
		virtual bool LogOut() const = 0;



		/*IsSignedIn Function
		* Checks wether the user is signed in or not
		* 
		* Returns true if signed in, false if not
		*/
		virtual bool IsSignedIn() const = 0;



		/*IsEmailVerified Function
		* Checks wether the email is verified or not
		* 
		* Returns true if the email is verified, false if not
		*/
		virtual bool IsEmailVerified() = 0;



		/*InitiateEmailAuth Function
		* Function to initiate the email authentication, this initates the provider to triger the otp or verification link
		* to be sent to the email provided. When this function is successfully executed, wait for the user to verify via email
		* sent to the user, and later on you can check with IsEmailVerified function, if the verification was a success.
		*
		* Params - 
		* exception - exception object reference
		* callback - the callback function to be called provided by the user
		*
		* Returns true if Authentication started successfully
		*/
		virtual bool InitiateEmailAuth(FireDriveException& exception, _OnCompletionCallBack callback) = 0;
	};


	//interface class for firbase User Details Querries
	class FireDriveUser {
	public:

		/*GetCurrentUserUID Function
		* Returns the UID of the current logged in user
		*
		* Returns the user UID in string if user is logged in, else returns NULL
		*/
		virtual std::shared_ptr<const std::string> GetCurrentUserUID() const = 0;


		/*GetCurrentUserDispName Function
		* Returns the display name of the current logged in user
		* 
		* Returns the user display name in string if user is logged in, else returns NULL
		*/
		virtual std::shared_ptr<const std::string> GetCurrentUserDispName() const = 0;


		/*ChangeUserDispName Function
		* Changes the current user display name
		* 
		* Parameters -
		* newDispName - the name to set as the new display name
		* exception - exception object
		* callback - the callback function passed by user
		* 
		* returns true if successfull, false if not
		*/
		virtual bool ChangeUserDispName(const char* newDispName, FireDriveException& exception, _OnCompletionCallBack callback = NULL) = 0;


		/*IsSignedIn Function
		* Checks wether the user is signed in or not
		*
		* Returns true if signed in, false if not
		*/
		virtual bool IsSignedIn() const = 0;
	};

	class FireDrive : public FireDriveAuthentication, public FireDriveUtility, public FireDriveUser {
	private:
		void* appOptions;
		void* app;
		volatile mutable void* user;

		std::shared_ptr<std::string> fileData;

		//blocked functions
		FireDrive(const FireDrive& obj);
		void operator=(const FireDrive& obj);

		//flag for signed in or not
		volatile bool FD_is_Signed_in = false;

	public:

		/*Constructor
		*
		* Creates the FireDrive object with the supplied arguments
		*
		* Parameters -
		* jsonConfigContent = the string content of the json config file
		* len = leave it as length of the string content
		*/
		FireDrive(const char* jsonConfigContent, size_t len);

		/*Constructor
		*
		* Creates the FireDrive object with the supplied config json
		*
		* Parameters -
		* pathToConfigJson = path to the config json file
		*/
		FireDrive(const char* pathToConfigJson);


		//destructor
		virtual ~FireDrive();


		//public functions

		/*getJsonConfigData Function
		*
		* Returns the json config file data as a shared pointer to a const string.
		*/
		std::shared_ptr<const std::string> getJsonConfigFileData();


		/*CreateUser Function
		* Creates a new User
		* 
		* Parameters -
		* email = the email to be used to login
		* password = the password to be set
		* exception = refference to a FireDriveException object
		* callback = a callback to be called at the end of the operation completion. Can be NULL.
		* 
		* Returns true if successful or false if not. Check the exception if false.
		*/
		bool CreateUser(const char* email, const char* password, FireDriveException& exception, _OnCompletionCallBack callback = NULL) override;



		/*SignInUser Function
		* Signs a user in, if provided correct password and email
		* 
		* Parameters - 
		* email = the email to be used while logging in
		* password = the password to be used while logging in
		* exception = refference to a FireDriveException object
		* callback = a callback function to be called at the end of the operation completion. Can be NULL.
		*/
		bool SignInUser(const char* email, const char* password, FireDriveException& exception, _OnCompletionCallBack callback = NULL) override;


		/*IsSignedIn Function
		* Checks wether the user is signed in or not
		*
		* Returns true if signed in, false if not
		*/
		bool IsSignedIn() const override;


		/*LogOut Function
		* Logs a user out, if currently logged in
		*
		* Returns true if user found and logged out successfully and also when no user is logged in, false if not able to log out the user.
		*/
		bool LogOut() const override;


		/*ChangeUserDispName Function
		* Changes the current user display name
		*
		* Parameters -
		* newDispName - the name to set as the new display name
		* exception - exception object
		* callback - the callback function passed by user
		* 
		* returns true if successfull, false if not
		*/
		bool ChangeUserDispName(const char* newDispName, FireDriveException& exception, _OnCompletionCallBack callback = NULL) override;

		/*GetCurrentUserUID Function
		* Returns the UID of the current logged in user
		*
		* Returns the user UID in string if user is logged in, else returns NULL
		*/
		std::shared_ptr<const std::string> GetCurrentUserUID() const override;


		/*GetCurrentUserDispName Function
		* Returns the display name of the current logged in user
		*
		* Returns the user display name in string if user is logged in, else returns NULL
		*/
		std::shared_ptr<const std::string> GetCurrentUserDispName() const override;


		/*IsEmailVerified Function
		* Checks wether the email is verified or not
		*
		* Returns true if the email is verified, false if not
		*/
		bool IsEmailVerified() override;


		/*InitiateEmailAuth Function
		* Function to initiate the email authentication, this initates the provider to triger the otp or verification link
		* to be sent to the email provided. When this function is successfully executed, wait for the user to verify via email
		* sent to the user, and later on you can check with IsEmailVerified function, if the verification was a success.
		*
		* Params -
		* exception - exception object reference
		* callback - the callback function to be called provided by the user
		*
		* Returns true if Authentication started successfully
		*/
		bool InitiateEmailAuth(FireDriveException& exception, _OnCompletionCallBack callback) override;
	};


};

#endif