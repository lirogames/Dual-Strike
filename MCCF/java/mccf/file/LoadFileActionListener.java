package mccf.file;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;

import javax.swing.JFileChooser;


public class LoadFileActionListener implements ActionListener {
	private final FileHandler fileHandler;
	
	LoadFileActionListener(final FileHandler fileHandler) {
		this.fileHandler = fileHandler;
	}

	@Override
	public void actionPerformed(final ActionEvent event) {
		File file;
		
		fileHandler.getView().makeWindowInactive();

		if(fileHandler.getFileChooser().showOpenDialog(fileHandler.getView().getWindow()) == JFileChooser.APPROVE_OPTION) {
			file = fileHandler.getFileChooser().getSelectedFile();
			
			fileHandler.load(file);
		}
	
		fileHandler.getView().makeWindowActive();
	}
}
