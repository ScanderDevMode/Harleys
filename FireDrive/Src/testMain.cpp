#include <stdio.h>

#include "FireDrive.h"





using namespace _FireDrive_;


//for now a global variable to be used as a flag to stop and start the waiting loops
//can be in the same scope as the function call, if used with a lambda function
bool waitPlease = true;

//test call back function
void onCreateCallBack(bool res, FireDriveException exception, std::shared_ptr<CallbackParams> params) {
	waitPlease = false;
	
	switch (params.get()->_funcID) {
	case FID_SIGNIN_USER:
		printf("User Signed In... \n");
		break;
	case FID_INITIATE_EMAIL_AUTH:
		printf("Authentication Email sent... \nPlease check your mail and click on the verification link.// \n");
		break;
	default:
		break;
	}
}


int main(int argc, char** argv) {
	//This is an example of synchronized programming using this api,
	//you can totally go with the 



	//you can hide the data like this - if you are going for a dll build
	const char* fileData = "{\"project_info\":{\"project_number\":\"944286589208\",\"project_id\":\"harleys-4e431\",\"storage_bucket\":\"harleys-4e431.appspot.com\"},\"client\":[{\"client_info\":{\"mobilesdk_app_id\":\"1:944286589208:android:a38fcecfed8f888900be6d\",\"android_client_info\":{\"package_name\":\"com.ThreeDSSofts.Harleys\"}},\"oauth_client\":[{\"client_id\":\"944286589208-jktrtfp0ir11hbhhmpbqlffjp6q2abk9.apps.googleusercontent.com\",\"client_type\":3}],\"api_key\":[{\"current_key\":\"AIzaSyCduPJviqlQn1S-zbPm56Zrwt-QXL2xYCk\"}],\"services\":{\"appinvite_service\":{\"other_platform_oauth_client\":[{\"client_id\":\"944286589208-jktrtfp0ir11hbhhmpbqlffjp6q2abk9.apps.googleusercontent.com\",\"client_type\":3},{\"client_id\":\"944286589208-rl1q45qndnofe3oe0vi94e8ufhop5pas.apps.googleusercontent.com\",\"client_type\":2,\"ios_info\":{\"bundle_id\":\"com.3dsSofts.Harleys\"}}]}}}],\"configuration_version\":\"1\"}";
	FireDriveException exception;


	FireDrive* fd = new FireDrive(fileData, strlen(fileData));

	//get the json config file data
	auto fileStr = fd->getJsonConfigFileData();

	//create a new user
	//fd->CreateUser("amitkrdas20@gmail.com", "am_p0qpl", exception, onCreateCallBack);

	//sign-in user
	fd->SignInUser("amitkrdas20@gmail.com", "am_p0qpl", exception, onCreateCallBack);
	

	//wait for the login to happen / same works for while creating a new user
	//use a time limit to wait for if you want, eventually also usually, if something goes wrong your callback will be called, with false in it
	//so you can put a loop with a bool set to true, and then set it to false from inside the callback
	while (!fd->IsSignedIn());


	//update the display name
	//fd->ChangeUserDispName("TheBigBoss", exception, onCreateCallBack);
	//while (waitPlease); //if you want to wait, better to wait before tryig to do something


	if (!fd->IsEmailVerified()) {
		waitPlease = true;
		fd->InitiateEmailAuth(exception, onCreateCallBack);
		while (waitPlease); //if you want to wait, better to wait before do to print something
	}


	if (!fd->IsEmailVerified())
		printf("Email failed to get Verified... \n");
	else
		printf("Email Verified... \n");



	//Print the user Details
	std::shared_ptr<const std::string> displayName = fd->GetCurrentUserDispName();
	std::shared_ptr<const std::string> uid = fd->GetCurrentUserUID();
	printf("User -> %s \n", displayName.get()->c_str());
	printf("UID -> %s \n", uid.get()->c_str());

	//sign out
	fd->LogOut();

	//delete and see the FireDrive class exiting
	delete fd;
}




//{
////shared_ptr tests
//
//	//CallbackParams _params(
//	//	""
//	//);
//	//std::shared_ptr<_FireDrive_::CallbackParams> params = std::make_shared<_FireDrive_::CallbackParams>(_params);
//
//}