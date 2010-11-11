import java.util.ArrayList;

/**
 * The MusicMatatu class stores Tracks and provides searching by both song name and artist names.
 * 
 * 
 */
public class MusicM {
	public static final String MYNAME = "Saul Lushion";
	private ArrayList songs = new ArrayList();

	/**
	 * This method adds the given argument Track to the MusicMatatu. 
	 * 
	 * @param t The track to be added.
	 */
	void addTrack(Track t) {
		if (!songs.contains(t))
			songs.add(t);
	}

	/**
	 * This method returns an array of ALL Tracks matching the given song name, or null if no such songs exist.
	 * There may be multiple artists with identical song names.
	 * 
	 * @param songName The name of the song to search for. 
	 * @return An array of songs exactly matching the argument song name or null if no such songs exist.
	 */
	Track[] getBySong(String songName) {
		ArrayList tempSongs = new ArrayList();
		for (int i = 0; i < this.numTracks(); i++) {
			Track tempTrack = this.get(i);
			if (tempTrack.getSongName().equals(songName))
				tempSongs.add(tempTrack);
		}
		if (tempSongs.size() > 0) {
			Track[] output = new Track[tempSongs.size()];
			for (int i = 0; i < tempSongs.size(); i++)
				output[i] = (Track) tempSongs.get(i);
			return output;
		} else
			return null;
	}

	/**
	 * This method returns an array of ALL Tracks matching the given artist name, or null if no such Tracks exist.
	 * 
	 * @param artistName The name of the artist to search for. 
	 * @return An array of songs exactly matching the argument artist name, or null if no such Tracks exist.
	 */
	Track[] getByArtist(String artistName) {
		ArrayList tempSongs = new ArrayList();
		for (int i = 0; i < this.numTracks(); i++) {
			Track tempTrack = this.get(i);
			if (tempTrack.getArtistName().equals(artistName))
				tempSongs.add(tempTrack);
		}
		if (tempSongs.size() > 0) {
			Track[] output = new Track[tempSongs.size()];
			for (int i = 0; i < tempSongs.size(); i++)
				output[i] = (Track) tempSongs.get(i);
			return output;
		} else
			return null;
	}

	/**
	 * Returns the Track in the MusicMatatu with the highest play count, or null if none exist.
	 * 
	 * @return The most-played song in the MusicMatatu
	 */
	Track topSong() {
		int maxPlayed = 0;
		Track maxTrack = null;
		for (int i = 0; i < this.numTracks(); i++) {
			Track candidate = this.get(i);
			if (candidate.getPlayCount() > maxPlayed ||
				(this.get(i).getPlayCount() == maxPlayed && 
						(maxTrack.getArtistName().compareTo(candidate.getArtistName()) > 0 ||
								(maxTrack.getArtistName().compareTo(candidate.getArtistName()) == 0 &&
								maxTrack.getSongName().compareTo(candidate.getSongName()) > 0)))){
				maxTrack = candidate;
				maxPlayed = maxTrack.getPlayCount();
			} 
		}
		return maxTrack;
	}

	/**
	 * Returns the number of Tracks in this MusicMatatu.
	 * 
	 * @return The number of Tracks stored in this MusicMatatu
	 */
	int numTracks() {
		return this.songs.size();
	}

	private Track get(int i) {
		if (i < songs.size())
			return (Track) songs.get(i);
		else
			return null;
	}
}

/**
 * The Track class represents a song that will be stored and returned by a MusicMatatu object.
 * 
 */
class Track {
	public static final String MYNAME = "Saul Lushion";

	private static int numTracks = 0;
	
	private String songName;

	private String artistName;

	private int songLength;

	private int playCount;

	/**
	 * Constructs a new Track object. 
	 * 
	 * @param artistName The name of this Track's artist.
	 * @param songName The name of this Track's song.
	 * @param songLength The length of this Track's song.
	 */
	Track(String artistName, String songName, int songLength) {
		this.songName = songName;
		this.artistName = artistName;
		this.songLength = songLength;
		Track.numTracks++;
	}

	/**
	 *  Increments this Track's play count.
	 */
	void play() {
		//assert (this.playCount > 0);
		this.playCount++;
	}

	/**
	 * Returns this Track's song name.
	 * 
	 * @return This Track's song name.
	 */
	String getSongName() {
		return this.songName;
	}

	/**
	 * Returns this Track's artist name.
	 * 
	 * @return This Track's artist name.
	 */
	String getArtistName() {
		return this.artistName;
	}

	/**
	 * Returns the length of this Track (in seconds)
	 * 
	 * @return The length of this Track (in seconds)
	 */
	int getSongLength() {
		return this.songLength;
	}

	/**
	 * Returns the number of times this Track has been played.
	 * 
	 * @return The number of times this Track has been played.
	 */
	int getPlayCount() {
		return this.playCount;
	}
	
	
	/**
	 * Returns the number of Track objects that have been instantiated.
	 * 
	 * @return Number of tracks that have been instantiated.
	 */
	static int numTracks() { 
		return Track.numTracks;
	}

	/**
	 * Returns a String representation of this track of the form "ArtistName - SongName"
	 * 
	 * @return A String representation of this Track of the form "ArtistName - SongName"
	 * @see java.lang.Object#toString()
	 */
	public String toString() {
		return this.getArtistName() + " - " + this.getSongName();
	}
}
