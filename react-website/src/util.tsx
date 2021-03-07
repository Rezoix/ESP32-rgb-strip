export const throttle = (func: Function, wait: number = 25) => {
  let isCalled = false;

  return (...args: any) => {
    if (!isCalled) {
      func(...args);
      isCalled = true;
      setTimeout(() => {
        isCalled = false;
      }, wait);
    }
  };
};
