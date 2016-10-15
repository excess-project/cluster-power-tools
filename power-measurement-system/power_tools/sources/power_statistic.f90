module power_statistic

use power_type
use power_timestep
use power_rawdata
use power_input_parameter
implicit none

contains 

subroutine power_statistic_median_value(step_left, step_right, values, indexes, median_value, err_code)
 integer(kind=ik),intent(in)                               :: step_left
 integer(kind=ik),intent(in)                               :: step_right
 real(kind=rdk),allocatable, dimension(:)   ,intent(in)     :: values
 integer(kind=ik),allocatable, dimension(:) ,intent(inout)    :: indexes
 real(kind=rdk), intent(out)                                :: median_value
 integer(kind=ik),intent(out)                              :: err_code
 integer(kind=ik)                              :: ii,jj,idx1,idx2, length
 logical :: swapped
 
 err_code = 0_ik;
 length=step_left+step_right+1_ik
 do
    swapped=.false.
    do jj= 1, length - 1_ik
       idx1=indexes(jj)
       idx2=indexes(jj+1_ik)
       if(values(idx1) .gt. values(idx2)) then
         indexes(jj)=idx2
         indexes(jj+1)=idx1
         swapped = .true.
       end if
     end do
     if (.not. swapped) exit;
  end do
  
  median_value = values(indexes(step_left+1_ik))
end subroutine

subroutine power_statistic_rawdata_filter_median(power_average_rawdata,power_rawdata,input_parameter,err_code)
  type(power_rawdata_type), intent(out)                      :: power_average_rawdata
  type(power_rawdata_type), intent(inout)                    :: power_rawdata
  type(power_input_parameter_type), intent(in)               :: input_parameter
  integer(kind=ik),intent(out)                               :: err_code
  integer(kind=ik) :: rawdata_length, ii, jj, step_left,step_right,kk
  integer(kind=ik) :: offset_idx
  real(kind=rdk) :: median_value
  real(kind=rdk),allocatable, dimension(:) :: temp_array
  integer(kind=ik),allocatable, dimension(:) :: temp_array_idx
  real(kind=rk) :: average
  
  
  err_code = 0_ik
  rawdata_length = size(power_rawdata%rawdata)
  allocate(power_average_rawdata%rawdata(rawdata_length))
  step_left = input_parameter%window_step_left
  step_right = input_parameter%window_step_right
  allocate(temp_array(step_left+step_right+1))
  allocate(temp_array_idx(step_left+step_right+1))
  power_average_rawdata%rawdata(1:step_left) = real(power_rawdata%rawdata(1:step_left),kind=rdk)  
  power_average_rawdata%rawdata((rawdata_length-step_right):rawdata_length) = &
    real(power_rawdata%rawdata((rawdata_length-step_right):rawdata_length),kind=rdk)  
  do kk=1_ik,input_parameter%num_operation_repeat
      write(*,'(A,I0)') "repetion no.:",kk
      do ii=step_left+1_ik,rawdata_length-step_right
        do jj=1,step_left+step_right+1_ik
          temp_array_idx(jj) = jj;
        end do
        do jj=1,step_left
          temp_array(step_left+1_ik-jj)= real(power_rawdata%rawdata(ii-jj),kind=rdk)
        end do
        temp_array(step_left+1_ik)= real(power_rawdata%rawdata(ii),kind=rdk)
        do jj=1,step_right
          temp_array(step_left+1_ik+jj)= real(power_rawdata%rawdata(ii+jj),kind=rdk)
        end do
        call power_statistic_median_value(step_left,step_right, temp_array,temp_array_idx,median_value,err_code)
        power_average_rawdata%rawdata(ii) = median_value 
      end do
      if (kk .lt. input_parameter%num_operation_repeat) then
        write(*,'(A,I0)') "copy the results for the repetition:",kk+1_ik
        call power_power_rawdata_average(average,power_rawdata,err_code)
        write(*,'(A,E16.6)') "average before:",average
        power_rawdata%rawdata=real(power_average_rawdata%rawdata,kind=rdk)
        call power_power_rawdata_average(average,power_rawdata,err_code)
        write(*,'(A,E16.6)') "average after:",average
      end if
  end do 
end subroutine power_statistic_rawdata_filter_median
 
 
 
subroutine power_statistic_rawdata_filter_mean(power_average_rawdata,power_rawdata,input_parameter,err_code)
  type(power_rawdata_type), intent(out)                      :: power_average_rawdata
  type(power_rawdata_type), intent(inout)                    :: power_rawdata
  type(power_input_parameter_type), intent(in)               :: input_parameter
  integer(kind=ik),intent(out)                               :: err_code
  integer(kind=ik) :: kk,rawdata_length, ii, jj, length, step_left,step_right
  real(kind=rk) :: average,sum_right, sum_left, sum
  
  err_code = 0_ik
  rawdata_length = size(power_rawdata%rawdata)
  allocate(power_average_rawdata%rawdata(rawdata_length))
  step_left = input_parameter%window_step_left
  step_right = input_parameter%window_step_right
  do kk=1_ik,input_parameter%num_operation_repeat
    do ii=1_ik,step_left
      sum_right = 0_rk
      sum_left = 0_rk
      length = 1_ik
      do jj=ii+1_ik,ii+step_right
        sum_right = sum_right + real(power_rawdata%rawdata(jj),kind=rk)
        length = length+ 1_ik
      end do
      do jj=1_ik,ii-1_ik
       sum_left = sum_left + real(power_rawdata%rawdata(jj),kind=rk)
       length = length+ 1_ik
      end do
      sum = sum_right + sum_left + power_rawdata%rawdata(ii)
      average = sum / length
      power_average_rawdata%rawdata(ii) = real(average,kind=rdk)
    end do
  
    do ii=step_left+1_ik,rawdata_length-step_right
      sum_right = 0_rk
      sum_left = 0_rk
      length = 1_ik
       do jj=ii+1_ik,ii+step_right
        sum_right = sum_right + real(power_rawdata%rawdata(jj),kind=rk)
        length = length+ 1_ik
      end do
      do jj=ii-step_left,ii-1_ik
       sum_left = sum_left + real(power_rawdata%rawdata(jj),kind=rk)
       length = length+ 1_ik
      end do
      sum = sum_right + sum_left + power_rawdata%rawdata(ii)
      average = sum / length
      power_average_rawdata%rawdata(ii) = real(average,kind=rdk)
    end do 
  
    do ii=rawdata_length-step_right + 1_ik, rawdata_length
      sum_right = 0_rk
      sum_left = 0_rk
      length = 1_ik
      do jj=ii-step_left,ii-1_ik
        sum_left = sum_left + real(power_rawdata%rawdata(jj),kind=rk)
        length = length+ 1_ik
      end do
      do jj=ii+1_ik,rawdata_length
        sum_right = sum_right + real(power_rawdata%rawdata(jj),kind=rk)
        length = length+ 1_ik
      end do
      sum = sum_right + sum_left + power_rawdata%rawdata(ii)
      average = sum / length
      power_average_rawdata%rawdata(ii) = real(average,kind=rdk)
    end do
    if (kk .lt. input_parameter%num_operation_repeat) then
      power_rawdata%rawdata=real(power_average_rawdata%rawdata,kind=rdk)
    end if
  end do
 end subroutine power_statistic_rawdata_filter_mean
 
 subroutine power_statistic_rawdata_filter_average(power_average_rawdata,power_rawdata,input_parameter,err_code)
  type(power_rawdata_type), intent(out)                      :: power_average_rawdata
  type(power_rawdata_type), intent(in)                       :: power_rawdata
  type(power_input_parameter_type), intent(in)               :: input_parameter
  integer(kind=ik),intent(out)                               :: err_code
  integer(kind=ik) :: rawdata_length, ii, jj, rawdata_rest_length, average_window, num_windows
  integer(kind=ik) :: start_idx, end_idx
  real(kind=rk) :: sum
  real(kind=rdk) :: average
  err_code = 0_ik
  rawdata_length = size(power_rawdata%rawdata)
  allocate(power_average_rawdata%rawdata(rawdata_length))
  average_window = input_parameter%window_step_left+input_parameter%window_step_right
  num_windows = rawdata_length / average_window
  rawdata_rest_length = rawdata_length - num_windows * average_window
  do ii=1_ik,num_windows
    sum = 0.0
    start_idx = (ii-1_ik)*average_window + 1_ik
    end_idx = (ii)*average_window
    do jj=start_idx,end_idx
      sum = sum + real(power_rawdata%rawdata(jj),kind=rk)
    end do
    average = real(sum / (end_idx-start_idx+1_ik),kind=rdk)
    power_average_rawdata%rawdata(start_idx:end_idx) = average
  end do
  sum = 0.0
  start_idx = average_window*num_windows+1_ik
  end_idx = rawdata_length
  do ii=start_idx,end_idx
    sum = sum + real(power_rawdata%rawdata(ii),kind=rk)
  end do
  average = real(sum / (end_idx-start_idx+1_ik),kind=rdk)
  power_average_rawdata%rawdata(start_idx:end_idx) = average
 end subroutine power_statistic_rawdata_filter_average

subroutine power_statistic_rawdata_filter_eq(power_eq_rawdata,power_rawdata,input_parameter,err_code)
  type(power_rawdata_type), intent(out)                      :: power_eq_rawdata
  type(power_rawdata_type), intent(in)                       :: power_rawdata
  type(power_input_parameter_type), intent(in)               :: input_parameter
  integer(kind=ik),intent(out)                               :: err_code
  integer(kind=ik) :: rawdata_length, ii, jj
  err_code = 0_ik
  rawdata_length = size(power_rawdata%rawdata)
  allocate(power_eq_rawdata%rawdata(rawdata_length))
  power_eq_rawdata%rawdata(1_ik:rawdata_length) =power_rawdata%rawdata(1_ik:rawdata_length)
 end subroutine power_statistic_rawdata_filter_eq



end module power_statistic
