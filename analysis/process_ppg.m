% -----------------------------------------------------------------------------
%                           Parameters
% -----------------------------------------------------------------------------
Fs      = 125;                  % sampling freq (used for Ts)
Ts      = 1/Fs;                 
order   = 2;                    % order of Butterworth filter
fc_low  = 0.4;                  % lower cutoff for BPF
fc_high = 5;                    % upper cutoff for BPF
chop    = 150;                  % number of samples to chop after filtering
thresh  = 0.4;                  % threshold for peaks of normalized derivative
filename= './data/recording_20161029_190929.txt';
% -----------------------------------------------------------------------------

% Load ppg data (infrared)
fid = fopen(filename);
s = textscan(fid,'%f','Delimiter','\n');
fclose(fid);
s = s{1}';

% Clean up the raw signal
s = s - mean(s);    % remove DC offset
s = s/max(abs(s));  % normalize

% generate Butterworth bandpass filter
W = [fc_low, fc_high] * 1/(Fs/2);
[b,a] = butter(order, W, 'bandpass');

s = filter(b,a,s);
s = s(chop:(end - chop));

% first order backward difference (derivative)
ds = zeros(1, length(s) - 1);
for k = 2:length(s)
    ds(k-1) = (s(k) - s(k - 1)); 
end

ds = ds/max(abs(ds));               % normalize

% Calculate heart rate
[pks, ixs] = findpeaks(ds);
beats = sum(pks > thresh);          % number of peaks above threshold
time_interval = (Ts*length(ds));    % approximate time to record sample

HR = 60*(beats/2)/time_interval     % beats/2 to accnt for dicrotic notch

