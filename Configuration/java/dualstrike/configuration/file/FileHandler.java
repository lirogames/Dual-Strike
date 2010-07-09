package dualstrike.configuration.file;

import java.awt.event.ActionListener;
import java.io.File;
import java.io.IOException;

import javax.swing.JFileChooser;
import javax.swing.filechooser.FileFilter;

import dualstrike.configuration.ConfigurationEditor;
import dualstrike.configuration.HexFilesUtility;
import dualstrike.configuration.MessageHelper;
import dualstrike.configuration.model.ByteLoadingException;

public class FileHandler {
	private static final String DEFAULT_EXTENSION = "mcc";
	private File file;
	private final ConfigurationEditor controller;
	private final JFileChooser fileChooser;
	
	public FileHandler(final ConfigurationEditor controller) {
		this.controller = controller;
		fileChooser = new JFileChooser();
		fileChooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
		fileChooser.setMultiSelectionEnabled(false);
		fileChooser.addChoosableFileFilter(
				new FileFilter() {
					@Override
					public boolean accept(File file) {
						if(file.isFile())
							return file.getName().endsWith("." + DEFAULT_EXTENSION);
						
						if(file.isDirectory())
							return true;
						
						return false;
					}

					@Override
					public String getDescription() {
						return "*." + DEFAULT_EXTENSION;
					}
					
				}
		);
	}
	
	public ActionListener createLoadFileActionListener() {
		return new LoadFileActionListener(this);
	}
	
	public ActionListener createForgetFileActionListener() {
		return new ForgetFileActionListener(this);
	}

	public ActionListener createSaveFileActionListener() {
		return new SaveFileActionListener(this, false);
	}

	public ActionListener createSaveAsFileActionListener() {
		return new SaveFileActionListener(this, true);
	}

	ConfigurationEditor getController() {
		return controller;
	}

	JFileChooser getFileChooser() {
		return fileChooser;
	}

	synchronized boolean isFileActive() {
		return file != null;
	}

	synchronized void load(final File file) {
		if(!file.exists()) {
			controller.showErrorDialog(MessageHelper.get(this, "fileMissingErrorTitle"), MessageHelper.get(this, "fileMissingErrorMessage", file));
			controller.setStatusLabelText(MessageHelper.get(this, "loadFileErrorStatus"));
		}
		else {
			try {
				byte[] data;
				
				data = HexFilesUtility.readSimpleHexFile(file);
				controller.getModel().loadBytes(data);
				this.file = file;
				controller.setWindowTitleAmendment(file.getAbsolutePath());
				controller.setStatusLabelText(MessageHelper.get(this, "fileLoadedStatus"));
			}
			catch(IOException e) {
				controller.showErrorDialog(MessageHelper.get(this, "fileLoadingErrorTitle"), MessageHelper.get(this, "fileLoadingErrorMessage", file, e.getLocalizedMessage()));
				controller.setStatusLabelText(MessageHelper.get(this, "loadFileErrorStatus"));
			}
			catch(ByteLoadingException e) {
				e.showErrorDialog(controller);
				controller.setStatusLabelText(MessageHelper.get(this, "loadFileErrorStatus"));
			}
		}
	}

	synchronized void forget() {
		if(file != null) {
			file = null;
			controller.setWindowTitleAmendment(null);
			controller.setStatusLabelText(MessageHelper.get(this, "fileForgottenStatus"));
		}
	}
	
	private static File addFileExtension(File file) {
		String name;
		boolean containsDot;
		boolean endsWithDot;
		boolean hasExtension;
		
		name = file.getName();
		containsDot = name.contains(".");
		endsWithDot = name.charAt(name.length() - 1) == '.';
		hasExtension = containsDot && !endsWithDot;
		
		if(!hasExtension)
			file = new File(file.getPath() + (endsWithDot ? "" : ".") + DEFAULT_EXTENSION);
		
		return file;
	}

	synchronized void save(File file) {
		try {
			byte[] data;
			
			file = addFileExtension(file);			
			data = controller.getModel().saveBytes();
			HexFilesUtility.saveSimpleHexFile(file, data);
			controller.setWindowTitleAmendment(file.getAbsolutePath());
			controller.setStatusLabelText(MessageHelper.get(this, "fileSavedStatus"));
		}
		catch(IOException e) {
			controller.showErrorDialog(MessageHelper.get(this, "fileSavingErrorTitle"), MessageHelper.get(this, "fileSavingErrorMessage", file, e.getLocalizedMessage()));
			controller.setStatusLabelText(MessageHelper.get(this, "saveFileErrorStatus"));
		}
	}

	synchronized void save() {
		save(file);
	}
}
