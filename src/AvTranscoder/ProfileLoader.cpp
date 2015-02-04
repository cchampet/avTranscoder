#include "ProfileLoader.hpp"

#include "common.hpp"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdexcept>
#include <dirent.h>

namespace avtranscoder
{

ProfileLoader::ProfileLoader( bool autoload )
{
	if( autoload )
		loadProfiles();
}

void ProfileLoader::loadProfile( const std::string& avProfileFileName )
{
	std::ifstream infile;
	infile.open( avProfileFileName.c_str(), std::ifstream::in );
	
	ProfileLoader::Profile customProfile;

	std::string line;
	while( std::getline( infile, line ) )
	{
		std::vector< std::string > keyValue;
		split( keyValue, line, "=" );
		if( keyValue.size() == 2 )
			customProfile[ keyValue.at( 0 ) ] = keyValue.at( 1 );
	}
	loadProfile( customProfile );
}

void ProfileLoader::loadProfiles( const std::string& avProfilesPath )
{	
	std::string realAvProfilesPath = avProfilesPath;
	if( realAvProfilesPath.empty() )
	{
		if( std::getenv( "AVPROFILES" ) )
			realAvProfilesPath = std::getenv( "AVPROFILES" );
		else
			return;
	}
	
	std::vector< std::string > paths;
	split( paths, realAvProfilesPath, ":" );
	for( std::vector< std::string >::iterator dirIt = paths.begin(); dirIt != paths.end(); ++dirIt )
	{
		std::vector< std::string > files;
		if( getFilesInDir( *dirIt, files ) != 0 )
			continue;

		for( std::vector< std::string >::iterator fileIt = files.begin(); fileIt != files.end(); ++fileIt )
		{
			const std::string absPath = ( *dirIt ) + "/" + ( *fileIt );
			try
			{
				loadProfile( absPath );
			}
			catch( const std::exception& e )
			{
				std::cout << e.what() << std::endl;
			}
		}
	}
}

void ProfileLoader::loadProfile( const Profile& profile )
{
	// check profile long name
	if( ! profile.count( constants::avProfileIdentificator ) )
	{
		throw std::runtime_error( "Warning: A profile has no name. It will not be loaded." );
	}

	// check profile type
	if( profile.count( constants::avProfileType ) == 0 )
	{
		throw std::runtime_error( "Warning: The profile " + profile.find( constants::avProfileIdentificator )->second + " has not type. It will not be loaded." );
	}

	// check complete profile
	bool isValid = false;
	std::string type( profile.find( constants::avProfileType )->second );
	if( type == constants::avProfileTypeFormat )
		isValid = checkFormatProfile( profile );
	else if( type == constants::avProfileTypeVideo )
		isValid = checkVideoProfile( profile );
	else if( type == constants::avProfileTypeAudio )
		isValid = checkAudioProfile( profile );

	if( isValid )
		_profiles.push_back( profile );
	else
		throw std::runtime_error( "Warning: The profile " + profile.find( constants::avProfileIdentificator )->second + " is invalid. It will not be loaded." );
}

const ProfileLoader::Profiles& ProfileLoader::getProfiles()
{
	return _profiles;
}

ProfileLoader::Profiles ProfileLoader::getFormatProfiles()
{
	Profiles profiles;

	for( Profiles::iterator it = _profiles.begin(); it != _profiles.end(); ++it )
	{
		if( (*it).find( constants::avProfileType )->second == constants::avProfileTypeFormat )
			profiles.push_back( *it );
	}

	return profiles;
}

ProfileLoader::Profiles ProfileLoader::getVideoProfiles()
{
	Profiles profiles;

	for( Profiles::iterator it = _profiles.begin(); it != _profiles.end(); ++it )
	{
		if( (*it).find( constants::avProfileType )->second == constants::avProfileTypeVideo )
			profiles.push_back( *it );
	}

	return profiles;
}

ProfileLoader::Profiles ProfileLoader::getAudioProfiles()
{
	Profiles profiles;

	for( Profiles::iterator it = _profiles.begin(); it != _profiles.end(); ++it )
	{
		if( (*it).find( constants::avProfileType )->second == constants::avProfileTypeAudio )
			profiles.push_back( *it );
	}

	return profiles;
}

ProfileLoader::Profile& ProfileLoader::getProfile( const std::string& avProfileIdentificator )
{
	for( Profiles::iterator it = _profiles.begin(); it != _profiles.end(); ++it )
	{
		if( (*it).find( constants::avProfileIdentificator )->second == avProfileIdentificator )
		{
			return (*it);
		}
	}
	throw std::runtime_error( "unable to find profile: " + avProfileIdentificator );
}


bool ProfileLoader::checkFormatProfile( const Profile& profileToCheck )
{
	bool isValid = true;

	if( ! profileToCheck.count( constants::avProfileIdentificator ) ||
		! profileToCheck.count( constants::avProfileIdentificatorHuman ) ||
		! profileToCheck.count( constants::avProfileType ) ||
		! profileToCheck.count( constants::avProfileFormat ) )
	{
		isValid = false;
	}

	return isValid;
}

bool ProfileLoader::checkVideoProfile( const Profile& profileToCheck )
{
	bool isValid = true;

	if( ! profileToCheck.count( constants::avProfileIdentificator ) ||
		! profileToCheck.count( constants::avProfileIdentificatorHuman ) ||
		! profileToCheck.count( constants::avProfileType ) ||
		! profileToCheck.count( constants::avProfileCodec ) )
	{
		isValid = false;
	}

	return isValid;
}

bool ProfileLoader::checkAudioProfile( const Profile& profileToCheck )
{
	bool isValid = true;

	if( ! profileToCheck.count( constants::avProfileIdentificator ) ||
		! profileToCheck.count( constants::avProfileIdentificatorHuman ) ||
		! profileToCheck.count( constants::avProfileType ) ||
		! profileToCheck.count( constants::avProfileCodec ) )
	{
		isValid = false;
	}

	return isValid;
}

void split( std::vector< std::string >& splitString, const std::string& inputString, const std::string& splitChars )
{
	char* part = strtok( const_cast<char*>( inputString.c_str() ), splitChars.c_str() );
	while( part != NULL )
	{
		splitString.push_back( std::string( part ) );
		part = strtok( NULL, splitChars.c_str() );
	}
}

int getFilesInDir( const std::string& dir, std::vector< std::string >& files )
{
	DIR *dp;
	struct dirent *dirp;
	if( ( dp  = opendir( dir.c_str() ) ) == NULL )
	{
		std::cerr << "Error(" << errno << ") opening " << dir << std::endl;
		return errno;
	}

	while( ( dirp = readdir( dp ) ) != NULL )
	{
		std::string filename( dirp->d_name );
		if( filename == "." || filename == ".." )
			continue;
		files.push_back( filename );
	}
	closedir( dp );
	return 0;
}

}
