import datetime


class TransactionInterval(object):
    MONTHLY = 'MONTHLY'
    WEEKLY  = 'WEEKLY'
    YEARLY  = 'YEARLY'

    @staticmethod
    def get_monthly_interval():
        return 31

    @staticmethod
    def get_weekly_interval():
        return 7

    @staticmethod
    def get_yearly_interval():
        return 365




class Transaction(object):
    def __init__(self, value: float):
        self._value = value

    def process(self):
        return self._value


class CreditTransaction(Transaction):
    def __init__(self, value: float):
        super().__init__(value)

    def process(self):
        return -self._value


class DebitTransaction(Transaction):
    def __init__(self, value: float):
        super().__init__(value)

    def process(self):
        return self._value


class Subscription(CreditTransaction):
    def __init__(self,
                 value: float,
                 start_date: datetime,
                 end_date: datetime,
                 interval: str):
        super().__init__(value)

        self._interval            = interval
        self._start_date          = start_date
        self._end_date            = end_date
        self._last_execution_date = None

    def _is_valid(self, current_date: datetime):
        return (current_date >= self._start_date) and
               (current_date < self._end_date)

    def _is_due(self, current_interval: int):
        last_execution = self._last_execution_date

        if last_execution is None:
            last_execution = self._start_date

        if self._is_valid(current_date):
            diff     = (current_date - last_execution).days
            duration = TransactionInterval.get_monthly_interval()

            if self._interval == TransactionInterval.WEEKLY:
                duration = TransactionInterval.get_weekly_interval()
            else:
                duration = TransactionInterval.get_yearly_interval()

            return diff >= duration
        else:
            return False

    def process(self, current_date: datetime):
        value = super().process()

