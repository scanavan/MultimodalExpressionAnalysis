function SubjectCount(d)
    direc = dir(d);
    id = fopen('FrameCount.txt', 'w');
    for i = 3:size(direc,1)
        %load file to get contents
        filePath = strcat(d, '/', direc(i).name);
        matFile = load(filePath);
        %parse file name to write
        list = strsplit(direc(i).name, '_');
        subject = list{1};
        task = strsplit(list{2}, '.');
        s = size(matFile.stereo, 2);
        fprintf(id, '%s %s %d\n', subject, task{1}, s);
    end
    fclose(id);
end